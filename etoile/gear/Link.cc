//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Link.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [sat jun 25 13:46:55 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Link.hh>
#include <etoile/gear/Nature.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Link::Link():
      Object(NatureLink),

      contents(NULL)
    {
    }

    ///
    /// the destructor
    ///
    Link::~Link()
    {
      // release the contents.
      if (this->contents != NULL)
	delete this->contents;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the contents along the the inherited object context.
    ///
    elle::Status	Link::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Link]" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the inherited object");

      // dump the contents.
      if (this->contents != NULL)
	{
	  if (this->contents->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the contents");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Contents] " << elle::none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the link.
    ///
    elle::Status	Link::Serialize(elle::Archive&		archive) const
    {
      enter();

      // serialize the contents.
      if (this->contents == NULL)
	{
	  // serialize the contents.
	  if (archive.Serialize(*this->contents) == elle::StatusError)
	    escape("unable to serialize the contents");
	}
      else
	{
	  // serialize 'none'.
	  if (archive.Serialize(elle::none) == elle::StatusError)
	    escape("unable to serialize 'none'");
	}

      leave();
    }

    ///
    /// this method extracts the link.
    ///
    elle::Status	Link::Extract(elle::Archive&		archive)
    {
      elle::Archive::Type	type;

      enter();

      // fetch the next element's type.
      if (archive.Fetch(type) == elle::StatusError)
	escape("unable to fetch the next element's type");

      // extract the access.
      if (type == elle::Archive::TypeNull)
	{
	  // extract 'none'.
	  if (archive.Extract(elle::none) == elle::StatusError)
	    escape("unable to extract 'none'");
	}
      else
	{
	  // allocate an contents.
	  this->contents = new nucleus::Contents<typename Link::C>;

	  // extract the contents.
	  if (archive.Extract(*this->contents) == elle::StatusError)
	    escape("unable to extract the contents");
	}

      leave();
    }

  }
}
