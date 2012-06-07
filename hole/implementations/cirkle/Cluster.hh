//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri aug 26 13:05:26 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_CLUSTER_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_CLUSTER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <hole/implementations/cirkle/RoutingTable.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Cluster:
        public elle::radix::Object
      {
      public:
        //
        // types
        //
        typedef std::list<elle::network::Locus>                  Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;

        //
        // constructors & destructors
        //
        ~Cluster();

        //
        // methods
        //
        elle::Status            Create(const RoutingTable&);

        //
        // interfaces
        //

        // object
        declare(Cluster);

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        // archivable
        //elle::Status            Serialize(elle::Archive&) const;
        //elle::Status            Extract(elle::Archive&);

        //
        // attributes
        //
        Container               container;
      };

    }
  }
}

#endif
#include <hole/implementations/cirkle/Cluster.hxx>
