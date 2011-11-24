//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [tue apr 13 15:27:20 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Hole.hh>
#include <Infinit.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "hole";

  ///
  /// this variable contains the network descriptor.
  ///
  lune::Descriptor		Hole::Descriptor;

  ///
  /// this variable contains the device passport.
  ///
  lune::Passport		Hole::Passport;

  ///
  /// this variable holds the hole implementation.
  ///
  Holeable*			Hole::Implementation = NULL;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the hole by allocating and initializing
  /// the implementation.
  ///
  elle::Status		Hole::Initialize()
  {
    nucleus::Network	network;
    elle::String	name;

    enter();

    // XXX everything must change!
    if (Infinit::Parser->Test("Network") == elle::StatusFalse)
      {
	name = "_______________PROTOTYPE_NETWORK_______________";
	// XXX name = "testdedingue";

	// trim string.
	name = name.substr(0, name.find_first_of(' '));

	// retrieve the descriptor.
	{
	  char		dsc[256];
	  char		command[256];

	  sprintf(command,
		  "mkdir -p %s/.infinit/networks/%s",
		  elle::System::Path::Home.c_str(),
		  name.c_str());
	  system(command);

	  sprintf(dsc, "%s/.infinit/networks/%s/%s.dsc",
		  elle::System::Path::Home.c_str(),
		  name.c_str(),
		  name.c_str());
	  sprintf(command,
		  "wget http://www.infinit.li/infinit/descriptors/%s.dsc "
		  "-O %s >/dev/null 2>&1",
		  name.c_str(),
		  dsc);
	  system(command);
	}

	// retrieve the passport.
	{
	  char		ppt[256];
	  char		command[256];

	  sprintf(ppt, "%s/.infinit/infinit.ppt",
		  elle::System::Path::Home.c_str());
	  sprintf(command,
		  "wget http://www.infinit.li/infinit/passports/infinit.ppt "
		  "-O %s >/dev/null 2>&1",
		  ppt);
	  system(command);
	}

	// retrieve the original network content.
	{
	  elle::Path	path;

	  // create the path.
	  if (path.Create(lune::Lune::Network::Shelter::Root) ==
	      elle::StatusError)
	    escape("unable to create the path");

	  // complete the path's pattern.
	  if (path.Complete(elle::Piece("%NETWORK%", name)) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // retrieve the shelter, if necessary.
	  if (elle::Directory::Exist(path) == elle::StatusFalse)
	    {
	      char	sht[256];
	      char	net[256];
	      char	command[256];

	      sprintf(sht, "%s/.infinit/networks/%s/shelter.tar.bz2",
		      elle::System::Path::Home.c_str(),
		      name.c_str());
	      sprintf(net, "%s/.infinit/networks/%s",
		      elle::System::Path::Home.c_str(),
		      name.c_str());

	      sprintf(command,
		      "wget http://www.infinit.li/infinit/shelters/%s.tar.bz2 "
		      "-O %s >/dev/null 2>&1",
		      name.c_str(),
		      sht);
	      system(command);

	      sprintf(command,
		      "tar xjvf %s -C %s >/dev/null 2>&1",
		      sht,
		      net);
	      system(command);

	      sprintf(command,
		      "rm -f %s >/dev/null 2>&1",
		      sht);
	      system(command);
	    }
	}

	// retrieve the list of hosts.
	{
	  elle::Host::Container	hosts;
	  elle::JSON::Document	doc;
	  elle::String		port;

	  if (elle::Variable::Convert(
		implementations::slug::Machine::Default::Port,
		port) == elle::StatusError)
	    escape("unable to convert the port");

	  if (elle::Host::Hosts(hosts) == elle::StatusError)
	    escape("unable to retrieve the list of host addresses");

	  for (auto s = hosts.begin(); s != hosts.end(); s++)
	    {
	      if ((*s).location.protocol() == ::QAbstractSocket::IPv6Protocol)
		continue;

	      if (doc.Append(
		    "loci",
		    elle::JSON::Bulk(
		      (*s).location.toString().toStdString() +
		      ":" +
		      port)) == elle::StatusError)
		escape("unable to append to the document");
	    }

	  if (elle::REST::PUT("infinit.li:12345/prototype/" + name,
			      doc) == elle::StatusError)
	    escape("unable to PUT the host's addresses");

	  if (elle::REST::GET("infinit.li:12345/prototype/" + name,
			      doc) == elle::StatusError)
	    escape("unable to PUT the host's addresses");

	  elle::Natural32	size;
	  elle::Natural32	i;
	  std::stringstream	ss;

	  if (doc.Size(size) == elle::StatusError)
	    escape("unable to retrieve the number of elements");

	  for (i = 0; i < size; i++)
	    {
	      elle::String	locus;

	      doc.Get(i, locus);

	      ss << locus;

	      if ((i + 1) < size)
		ss << " ";
	    }

	  lune::Descriptor	descriptor;

	  if (descriptor.Load(name) == elle::StatusError)
	    escape("unable to load the descriptor");

	  if (descriptor.Pull() == elle::StatusError)
	    escape("unable to pull the descriptor's attributes");

	  if (descriptor.Set("slug", "hosts",
			     ss.str()) == elle::StatusError)
	    escape("unable to set the hosts list");

	  if (descriptor.Push() == elle::StatusError)
	    escape("unable to push the descriptor");

	  if (descriptor.Store(name) == elle::StatusError)
	    escape("unable to store the descriptor");
	}
      }
    else
      {
	// retrieve the network name.
	if (Infinit::Parser->Value("Network", name) == elle::StatusError)
	  {
	    // display the usage.
	    Infinit::Parser->Usage();

	    escape("unable to retrieve the network name");
	  }
      }

    // disable the meta logging.
    if (elle::Meta::Disable() == elle::StatusError)
      escape("unable to disable the meta logging");

    //
    // retrieve the descriptor.
    //
    {
      // does the network exist.
      if (Hole::Descriptor.Exist(name) == elle::StatusFalse)
	escape("this network does not seem to exist");

      // load the descriptor.
      if (Hole::Descriptor.Load(name) == elle::StatusError)
	escape("unable to load the descriptor");

      // pull the attributes.
      if (Hole::Descriptor.Pull() == elle::StatusError)
	escape("unable to pull the descriptor's attributes");

      // validate the descriptor.
      if (Hole::Descriptor.Validate(Infinit::Authority) == elle::StatusError)
	escape("unable to validate the descriptor");
    }

    //
    // retrieve the passport.
    //
    {
      // does the network exist.
      if (Hole::Passport.Exist() == elle::StatusFalse)
	escape("the device passport does not seem to exist");

      // load the passport.
      if (Hole::Passport.Load() == elle::StatusError)
	escape("unable to load the passport");

      // validate the passport.
      if (Hole::Passport.Validate(Infinit::Authority) == elle::StatusError)
	escape("unable to validate the passport");
    }

    // enable the meta logging.
    if (elle::Meta::Enable() == elle::StatusError)
      escape("unable to enable the meta logging");

    // create the network instance.
    if (network.Create(name) == elle::StatusError)
      escape("unable to create the network instance");

    // create the holeable depending on the model.
    switch (Hole::Descriptor.model.type)
      {
      case Model::TypeLocal:
	{
	  // allocate the instance.
	  Hole::Implementation =
	    new implementations::local::Implementation(network);

	  break;
	}
      case Model::TypeRemote:
	{
	  // allocate the instance.
	  Hole::Implementation =
	    new implementations::remote::Implementation(network);

	  break;
	}
      case Model::TypeSlug:
        {
	  // allocate the instance.
          Hole::Implementation =
            new implementations::slug::Implementation(network);

          break;
        }
      case Model::TypeCirkle:
	{
	  /* XXX
	  // allocate the instance.
	  Hole::Implementation =
	    new implementations::cirkle::Implementation(network);
	  */

	  break;
	}
      default:
	escape("unknown or not-yet-supported model '%u'",
	       Hole::Descriptor.model.type);
      }

    // join the network
    if (Hole::Implementation->Join() == elle::StatusError)
      escape("unable to join the network");

    leave();
  }

  ///
  /// this method cleans the hole.
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status		Hole::Clean()
  {
    enter();

    // leave the network
    if (Hole::Implementation->Leave() == elle::StatusError)
      escape("unable to leave the network");

    // delete the implementation.
    delete Hole::Implementation;

    leave();
  }

  ///
  /// this method sets up the hole-specific options.
  ///
  elle::Status		Hole::Options()
  {
    enter();

    // register the option.
    if (Infinit::Parser->Register(
	  "Network",
	  'n',
	  "network",
	  "specifies the name of the network",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    leave();
  }

  ///
  /// this method returns the address of the root block i.e the origin.
  ///
  elle::Status		Hole::Origin(nucleus::Address&		address)
  {
    enter();

    // return the address.
    address = Hole::Descriptor.root;

    leave();
  }

  ///
  /// this method stores the given block.
  ///
  elle::Status		Hole::Push(const nucleus::Address&	address,
				   const nucleus::Block&	block)
  {
    enter();

    // XXX check the block's footprint which should not exceed Extent

    // forward the request depending on the nature of the block which
    // the address indicates.
    switch (address.family)
      {
      case nucleus::FamilyContentHashBlock:
	{
	  const nucleus::ImmutableBlock*	ib;

	  // cast to an immutable block.
	  ib = static_cast<const nucleus::ImmutableBlock*>(&block);

	  // store the immutable block.
	  if (Hole::Implementation->Put(address, *ib) == elle::StatusError)
	    escape("unable to put the block");

	  break;
	}
      case nucleus::FamilyPublicKeyBlock:
      case nucleus::FamilyOwnerKeyBlock:
      case nucleus::FamilyImprintBlock:
	{
	  const nucleus::MutableBlock*		mb;

	  // cast to a mutable block.
	  mb = static_cast<const nucleus::MutableBlock*>(&block);

	  // store the mutable block.
	  if (Hole::Implementation->Put(address, *mb) == elle::StatusError)
	    escape("unable to put the block");

	  break;
	}
      default:
	{
	  escape("unknown block family '%u'",
		 address.family);
	}
      }

    leave();
  }

  ///
  /// this method returns the block associated with the given address.
  ///
  elle::Status		Hole::Pull(const nucleus::Address&	address,
				   const nucleus::Version&	version,
				   nucleus::Block&		block)
  {
    enter();

    // forward the request depending on the nature of the block which
    // the addres indicates.
    switch (address.family)
      {
      case nucleus::FamilyContentHashBlock:
	{
	  nucleus::ImmutableBlock*	ib;

	  // cast to an immutable block.
	  ib = static_cast<nucleus::ImmutableBlock*>(&block);

	  // retrieve the immutable block.
	  if (Hole::Implementation->Get(address, *ib) == elle::StatusError)
	    escape("unable to get the block");

	  break;
	}
      case nucleus::FamilyPublicKeyBlock:
      case nucleus::FamilyOwnerKeyBlock:
      case nucleus::FamilyImprintBlock:
	{
	  nucleus::MutableBlock*	mb;

	  // cast to a mutable block.
	  mb = static_cast<nucleus::MutableBlock*>(&block);

	  // retrieve the mutable block.
	  if (Hole::Implementation->Get(address, version,
					*mb) == elle::StatusError)
	    escape("unable to get the block");

	  break;
	}
      default:
	{
	  escape("unknown block family '%u'",
		 address.family);
	}
      }

    leave();
  }

  ///
  /// this method removes the block associated with the given address.
  ///
  elle::Status		Hole::Wipe(const nucleus::Address&	address)
  {
    enter();

    // forward the kill request to the implementation.
    if (Hole::Implementation->Kill(address) == elle::StatusError)
      escape("unable to erase the block");

    leave();
  }

}
