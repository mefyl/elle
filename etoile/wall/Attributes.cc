//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed mar 31 19:26:06 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Attributes.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/Attributes.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method sets an attribute for the given object.
    ///
    elle::Status	Attributes::Set(
			  const gear::Identifier&		identifier,
			  const elle::String&			name,
			  const elle::String&			value)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Attributes::Set()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::S	section(
	elle::Hurdle::L(
	  elle::Hurdle::C(&elle::Hurdle::Lock, &scope->hurdle),
	  elle::ModeWrite),
	elle::Hurdle::U(
	  elle::Hurdle::C(&elle::Hurdle::Unlock, &scope->hurdle),
	  elle::ModeWrite));

      // protect the access.
      section.Enter();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// apply the set automaton on the context.
	if (automaton::Attributes::Set(*context,
				       name,
				       value) == elle::StatusError)
	  escape("unable to set the attribute");

	// set the actor's state.
	actor->state = gear::Actor::StateUpdated;
      }
      section.Leave();

      leave();
    }

    ///
    /// this method returns the caller the trait associated with
    /// the given name.
    ///
    elle::Status	Attributes::Get(
			  const gear::Identifier&		identifier,
			  const elle::String&			name,
			  nucleus::Trait*&			trait)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Attributes::Get()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::S	section(
	elle::Hurdle::L(
	  elle::Hurdle::C(&elle::Hurdle::Lock, &scope->hurdle),
	  elle::ModeRead),
	elle::Hurdle::U(
	  elle::Hurdle::C(&elle::Hurdle::Unlock, &scope->hurdle),
	  elle::ModeRead));

      // protect the access.
      section.Enter();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// apply the get automaton on the context.
	if (automaton::Attributes::Get(*context,
				       name,
				       trait) == elle::StatusError)
	  escape("unable to get the attribute");
      }
      section.Leave();

      leave();
    }

    ///
    /// this method returns all the attributes.
    ///
    elle::Status	Attributes::Fetch(
			  const gear::Identifier&		identifier,
			  nucleus::Range<nucleus::Trait>&	range)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Attributes::Fetch()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::S	section(
	elle::Hurdle::L(
	  elle::Hurdle::C(&elle::Hurdle::Lock, &scope->hurdle),
	  elle::ModeRead),
	elle::Hurdle::U(
	  elle::Hurdle::C(&elle::Hurdle::Unlock, &scope->hurdle),
	  elle::ModeRead));

      // protect the access.
      section.Enter();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// apply the fetch automaton on the context.
	if (automaton::Attributes::Fetch(*context,
					 range) == elle::StatusError)
	  escape("unable to fetch the attribute");
      }
      section.Leave();

      leave();
    }

    ///
    /// this method removes the given attribute from the list.
    ///
    elle::Status	Attributes::Omit(
			  const gear::Identifier&		identifier,
			  const elle::String&			name)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Attributes::Omit()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::S	section(
	elle::Hurdle::L(
	  elle::Hurdle::C(&elle::Hurdle::Lock, &scope->hurdle),
	  elle::ModeWrite),
	elle::Hurdle::U(
	  elle::Hurdle::C(&elle::Hurdle::Unlock, &scope->hurdle),
	  elle::ModeWrite));

      // protect the access.
      section.Enter();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// apply the omit automaton on the context.
	if (automaton::Attributes::Omit(*context,
					name) == elle::StatusError)
	  escape("unable to omit the attribute");

	// set the actor's state.
	actor->state = gear::Actor::StateUpdated;
      }
      section.Leave();

      leave();
    }

  }
}
