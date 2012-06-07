#include <iostream>
#include <stdexcept>

#include <QDir>

#include "plasma/common/resources.hh"

#include "IdentityUpdater.hh"

#include <lune/Identity.hh>
#include <lune/Passport.hh>

using namespace plasma::updater;

//
// ---------- contructors & descructors ---------------------------------------
//

IdentityUpdater::IdentityUpdater(QApplication& app) :
  _api(app),
  _loginDialog()
{}

//
// ---------- methods  --------------------------------------------------------
//

void IdentityUpdater::Start()
{
  std::cout << "IdentityUpdater::Start()\n";
  this->_loginDialog.show();
  this->connect(
    &this->_loginDialog, SIGNAL(doLogin(std::string const&, std::string const&)),
    this, SLOT(_DoLogin(std::string const&, std::string const&))
  );
}
void IdentityUpdater::_DoLogin(std::string const& login,
                               std::string const& password)
{
  if (!login.size()) // XXX || !password.size())
    {
      this->_loginDialog.SetErrorMessage("Wrong login/password");
      this->_loginDialog.show();
      return;
    }

    {
      std::cout << "LOGIN NOW !\n";
      this->_api.Login(
          login, password,
          std::bind(&IdentityUpdater::_OnLogin, this, password, std::placeholders::_1),
          std::bind(&IdentityUpdater::_OnError, this, std::placeholders::_1, std::placeholders::_2)
      );
    }
  this->_loginDialog.setDisabled(true);
  this->_loginDialog.show();
}

void IdentityUpdater::_OnLogin(std::string const& password,
                               meta::LoginResponse const& response)
{
  this->_loginDialog.setDisabled(false);
  if (!response.success)
    {
      std::cerr << "Login error: " << response.error << '\n';
      this->_loginDialog.SetErrorMessage(response.error);
      return;
    }
  std::cout << "Login success: "
            << response.fullname << ' '
            << response.email << ' '
            << response.token
            << '\n';

  this->_identity = this->_DecryptIdentity(password, response.identity);

  this->_token = response.token;

  QDir homeDirectory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));

  // Create the file infinit.idy
  if (!homeDirectory.exists("infinit.idy"))
      this->_StoreIdentity(response.identity);

  // Create the file infinit.dic
    {
      QFile f(homeDirectory.filePath("infinit.dic"));
      if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
          f.write("0b000b00"); // XXX This is an empty dic,
          f.close();
        }
      else
        std::cerr << "Could not create 'infinit.idy'.\n";
    }

  // Create the file infinit.ppt
    {
      if (!homeDirectory.exists("infinit.ppt"))
        {
          std::cout << "Checking out a new passport.\n";
          this->_CreateDevice();
        }
      else
        {
          std::cout << "Found a passport file.\n";
          this->_UpdateDevice();
        }
    }

  this->_loginDialog.hide();
}

// XXX try to read infinit.idy and use password to decrypt it
void IdentityUpdater::_OnError(meta::MetaClient::Error error,
                               std::string const& error_string)
{
  std::cout << "ERROR: " << (int)error << ": " << error_string << "\n";
  if (error == meta::MetaClient::Error::ConnectionFailure)
    {
      this->_loginDialog.SetErrorMessage(
        "An error occured, please check your internet connection"
      );
      this->_loginDialog.setDisabled(false);
    }
  else if (error == meta::MetaClient::Error::ServerError)
    {
      this->_loginDialog.SetErrorMessage(error_string);
      this->_loginDialog.setDisabled(false);
    }
  else
    {
      std::cerr << "Got a very unexpected error: " << error_string << "\n";
      Q_EMIT identityUpdated(false);
    }
}




#include "lune/Identity.hh"
#include "lune/Passport.hh"
#include "elle/network/Host.hh"

void IdentityUpdater::_OnDeviceCreated(meta::CreateDeviceResponse const& res)
{
  std::cout << "Created device " << res.created_device_id
            << " with passport: " << res.passport << "\n";

  lune::Passport passport;

  if (passport.Restore(res.passport) == elle::Status::Error)
    throw std::runtime_error("Cannot load the passport");
  if (passport.Store() == elle::Status::Error)
    throw std::runtime_error("Cannot save the passport");

  Q_EMIT identityUpdated(true);
}

void IdentityUpdater::_OnDeviceUpdated(meta::UpdateDeviceResponse const& res)
{
  std::cout << "Updated device " << res.updated_device_id
            << " with passport: " << res.passport << "\n";

  lune::Passport passport;

  // XXX check the old passport here ?

  if (passport.Restore(res.passport) == elle::Status::Error)
    throw std::runtime_error("Cannot load the passport");
  if (passport.Store() == elle::Status::Error)
    throw std::runtime_error("Cannot save the passport");

  Q_EMIT identityUpdated(true);

}

std::string IdentityUpdater::_DecryptIdentity(std::string const& password,
                                              std::string const& identityString)
{
  elle::Unique        id;
  lune::Identity      identity;

  if (identity.Restore(identityString)  == elle::Status::Error ||
      identity.Decrypt(password)        == elle::Status::Error ||
      identity.Clear()                  == elle::Status::Error ||
      identity.Save(id)                 == elle::Status::Error
      )
    {
      show();
      std::cerr << "Couldn't decrypt the identity file !\n"; // XXX
    }

  return id;
}

void IdentityUpdater::_StoreIdentity(std::string const& identityString)
{
  lune::Identity        identity;

  if (identity.Restore(identityString)  == elle::Status::Error ||
      identity.Store()                  == elle::Status::Error)
    {
      show();
      throw std::runtime_error("Cannot save the identity file.\n");
    }
}

namespace
{
    std::string get_local_address()
    {
      elle::network::Host::Container hosts;

      if (elle::network::Host::Hosts(hosts) == elle::Status::Error)
        throw std::runtime_error("Couldn't retreive host list");

      if (!hosts.size())
        throw std::runtime_error("No usable host found !");

      std::string host;
      hosts[0].Convert(host);
      return host;
    }
}

void IdentityUpdater::_UpdateDevice()
{
  lune::Passport passport;

  if (passport.Load() == elle::Status::Error)
    {
      show();
      throw std::runtime_error("Couldn't load the passport file :'(");
    }

  std::string host = get_local_address();
  this->_api.UpdateDevice(
      passport.id,
      nullptr,
      host.c_str(),
      1912,
      boost::bind(&IdentityUpdater::_OnDeviceUpdated, this, _1),
      boost::bind(&IdentityUpdater::_OnError, this, _1, _2)
  );
}

void IdentityUpdater::_CreateDevice()
{
  std::string host = get_local_address();

  std::cout << "Registering host: '" << host << "'\n";

  // XXX should be done in infinit instance
  this->_api.CreateDevice(
      "default device name", host, 1912,
      boost::bind(&IdentityUpdater::_OnDeviceCreated, this, _1),
      boost::bind(&IdentityUpdater::_OnError, this, _1, _2)
  );
}
