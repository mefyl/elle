#include <iostream>
#include <sstream>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>
#include <elle/system/System.hh>

#include <elle/utility/Unicode.hh>

#include <etoile/path/Way.hh>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null way.
    ///
    const Way                   Way::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor
    ///
    Way::Way()
    {
    }

    ///
    /// copy constructor
    ///
    Way::Way(const Way&                                         way):
      Object(way),

      path(way.path)
    {
    }

    ///
    /// character constructor
    ///
    Way::Way(const elle::Character&                             character):
      path(1, character)
    {
    }

    ///
    /// string constructor
    ///
    Way::Way(const elle::String&                                string):
      path(string)
    {
    }


    ///
    /// wide char constructor
    ///
    Way::Way(const wchar_t *                                    u16_str):
      path()
    {
      char *    str      = NULL;
      ssize_t   str_size = 0;

      // convert the wchar string.
      if (elle::utility::Utf16To8(u16_str,
                                  -1,
                                  &str,
                                  &str_size) == elle::Status::Error)
        fail("failed to convert the path to uft8");

      // assign the string.
      path.assign(str, str_size);

      // release the temporary string.
      free(str);
    }


    ///
    /// this constructor creates a way but returns the last element of the path
    /// in _name_.
    ///
    Way::Way(const Way&                                         way,
             Slice&                                             name):
      path()
    {
      elle::String::size_type   last_slash;

      // clear the name, just in case.
      name.clear();

      // find the position.
      last_slash = way.path.rfind(elle::system::System::Path::Separator);

      // check if way.path contains at least a slash
      if (last_slash == elle::String::npos)
        return;

      // extract path and name
      path.assign(way.path.substr(0, last_slash == 0 ? 1 : last_slash));
      name.assign(way.path.substr(last_slash + 1, way.path.npos));
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the length of the way.
    ///
    elle::Status        Way::Capacity(Length&                   length) const
    {
      // return the length.
      length = static_cast<Length>(this->path.length());

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Way::operator==(const Way&              element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the attributes..
      if (this->path != element.path)
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Way, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the object.
    ///
    elle::Status        Way::Dump(elle::Natural32       margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Way] " << this->path << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    //elle::Status        Way::Serialize(elle::Archive&   archive) const
    //{
    //  // serialize the target.
    //  if (archive.Serialize(this->path) == elle::Status::Error)
    //    escape("unable to serialize the path");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the object.
    /////
    //elle::Status        Way::Extract(elle::Archive&     archive)
    //{
    //  // extract the target.
    //  if (archive.Extract(this->path) == elle::Status::Error)
    //    escape("unable to extract the path");

    //  return elle::Status::Ok;
    //}

  }
}
