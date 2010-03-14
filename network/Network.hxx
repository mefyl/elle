//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.hxx
//
// created       julien quintard   [wed feb  3 16:05:34 2010]
// updated       julien quintard   [thu mar 11 17:26:31 2010]
//

#ifndef ELLE_NETWORK_NETWORK_HXX
#define ELLE_NETWORK_NETWORK_HXX

namespace elle
{
  namespace network
  {

//
// ---------- network ---------------------------------------------------------
//

    ///
    /// this method registers a callback associated with a set of types so that
    /// whenever a message is received, these types are extracted and the
    /// callback is triggered.
    ///
    template <const Tag G>
    Status		Network::Register(Callback&		callback)
    {
      Selectionoid< Message<G>::P::Quantum,
	            typename Message<G>::P >*	selectionoid;
      std::pair<Network::Iterator, Boolean>	result;

      enter(instance(selectionoid));

      // lock in reading.
      Network::Control.Lock(ModeRead);
      {
	// check if this type has already been registed.
	if (Network::Callbacks.find(G) != Network::Callbacks.end())
	  {
	    Network::Control.Unlock();
	    escape("unable to register an already registered type");
	  }
      }
      Network::Control.Unlock();

      // create a new selectionoid.
      selectionoid = new Selectionoid< Message<G>::P::Quantum,
                                       typename Message<G>::P >(callback);

      // lock in writing.
      Network::Control.Lock(ModeWrite);
      {
	// insert the callback in the container.
	result = Network::Callbacks.insert(
		   std::pair<Tag,
		             Network::Functionoid*>(G,
						    selectionoid));
      }
      Network::Control.Unlock();

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the callback in the container");

      // waive the selectionoid tracking.
      waive(selectionoid);

      leave();
    }

//
// ---------- selectonoids ----------------------------------------------------
//

    ///
    /// these selectionoids must be cloned depending on the number of
    /// parameters they take.
    ///

    ///
    /// empty selectionoid.
    ///
    template <typename P>
    class Network::Selectionoid<0, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	enter();

	// unpack the data.
	if (P::Unpack(data) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger() == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// single-parameter selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<1, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// two-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<2, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// three-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<3, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;
	typename P::P3	o3;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2, o3) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2, o3) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// four-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<4, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;
	typename P::P3	o3;
	typename P::P4	o4;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2, o3, o4) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2, o3, o4) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// five-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<5, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;
	typename P::P3	o3;
	typename P::P4	o4;
	typename P::P5	o5;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2, o3, o4, o5) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2, o3, o4, o5) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// six-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<6, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;
	typename P::P3	o3;
	typename P::P4	o4;
	typename P::P5	o5;
	typename P::P6	o6;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2, o3, o4, o5,
		      o6) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2, o3, o4, o5,
				    o6) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// seven-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<7, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;
	typename P::P3	o3;
	typename P::P4	o4;
	typename P::P5	o5;
	typename P::P6	o6;
	typename P::P7	o7;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2, o3, o4, o5,
		      o6, o7) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2, o3, o4, o5,
				    o6, o7) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// eight-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<8, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;
	typename P::P3	o3;
	typename P::P4	o4;
	typename P::P5	o5;
	typename P::P6	o6;
	typename P::P7	o7;
	typename P::P8	o8;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2, o3, o4, o5,
		      o6, o7, o8) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2, o3, o4, o5,
				    o6, o7, o8) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

    ///
    /// nine-parameters selectoinoid.
    ///
    template <typename P>
    class Network::Selectionoid<9, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid(Callback&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call(Data&				data) const
      {
	typename P::P1	o1;
	typename P::P2	o2;
	typename P::P3	o3;
	typename P::P4	o4;
	typename P::P5	o5;
	typename P::P6	o6;
	typename P::P7	o7;
	typename P::P8	o8;
	typename P::P9	o9;

	enter();

	// unpack the data.
	if (P::Unpack(data,
		      o1, o2, o3, o4, o5,
		      o6, o7, o8, o9) == StatusError)
	  escape("unable to unpack the data");

	// trigger the callback.
	if (this->callback.Trigger(o1, o2, o3, o4, o5,
				    o6, o7, o8, o9) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	enter();

	// dump the callback.
	if (this->callback.Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback&		callback;
    };

  }
}

#endif
