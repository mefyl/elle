#include <horizon/linux/FUker.hh>
#include <horizon/linux/FUSE.hh>
#include <horizon/operations.hh>

#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Scheduler.hh>
#include <elle/concurrency/Callback.hh>

#include <hole/Hole.hh>

#include <Infinit.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <elle/idiom/Close.hh>
# include <boost/function.hpp>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# include <boost/preprocessor/seq/for_each.hpp>
# include <boost/preprocessor/seq/for_each_i.hpp>
# include <boost/preprocessor/seq/pop_front.hpp>
# include <pthread.h>
# include <sys/mount.h>
# include <sys/param.h>
# include <sys/statfs.h>
# include <fuse/fuse_lowlevel.h>
#include <elle/idiom/Open.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.FUSE");

namespace horizon
{
  namespace linux
  {
    // XXX
    typedef struct ::timespec timespec2[2];

    ///
    /// this is the identifier of the thread which is spawn so as to start
    /// FUSE.
    ///
    /// note that creating a specific thread is required because the call
    /// to fuse_main() never returns.
    ///
    ::pthread_t                 FUker::Thread;

    ///
    /// this attribute represents the main structure for manipulating a
    /// FUSE-mounted point.
    ///
    struct ::fuse*              FUker::FUSE = nullptr;

    /// The callbacks below are triggered by FUSE whenever a kernel
    /// event occurs.
    ///
    /// Note that every one of the callbacks run in a specific thread.
    ///
    /// the purpose of the code is to create an event, inject it in
    /// the event loop so that the Broker can treat it in a fiber and
    /// possible block.
    ///
    /// since the thread must not return until the event is treated,
    /// the following relies on a semaphore by blocking on it. once
    /// the event handled, the semaphore is unlocked, in which case
    /// the thread is resumed and can terminate by returning the
    /// result of the upcall.

#define INFINIT_FUSE_FORMALS(R, Data, I, Elem)  \
    , Elem BOOST_PP_CAT(a, BOOST_PP_INC(I))

#define INFINIT_FUSE_EFFECTIVE(R, Data, I, Elem)        \
    , BOOST_PP_CAT(a, I)

#define INFINIT_FUSE_BOUNCER(Name, Args)                                \
    static int                                                          \
    Name(BOOST_PP_SEQ_HEAD(Args) a0                                     \
         BOOST_PP_SEQ_FOR_EACH_I(INFINIT_FUSE_FORMALS, _,               \
                                 BOOST_PP_SEQ_POP_FRONT(Args)))         \
    {                                                                   \
      return elle::concurrency::scheduler().mt_run<int>                 \
        (BOOST_PP_STRINGIZE(Name),                                      \
         boost::bind(FUSE::Operations.Name                              \
                     BOOST_PP_SEQ_FOR_EACH_I(INFINIT_FUSE_EFFECTIVE,    \
                                             _, Args)));                \
    }                                                                   \

#define INFINIT_FUSE_BOUNCER_(R, Data, Elem)                            \
    INFINIT_FUSE_BOUNCER(BOOST_PP_TUPLE_ELEM(2, 0, Elem),               \
                         BOOST_PP_TUPLE_ELEM(2, 1, Elem))               \

    BOOST_PP_SEQ_FOR_EACH(INFINIT_FUSE_BOUNCER_, _, INFINIT_FUSE_COMMANDS)

#undef INFINIT_FUSE_BOUNCER_
#undef INFINIT_FUSE_BOUNCER
#undef INFINIT_FUSE_FORMALS
#undef INFINIT_FUSE_EFFECTIVE

    ///
    /// this method represents the entry point of the FUSE-specific thread.
    ///
    /// this method is responsible for starting FUSE.
    ///
    void*               FUker::Setup(void*)
    {
      //
      // build the arguments.
      //
      // note that the -h option can be passed in order to display all
      // the available options including the threaded, debug, file system
      // name, file system type etc.
      //
      // for example the -d option could be used instead of -f in order
      // to activate the debug mode.
      //
      elle::String      ofsname("-ofsname=" +
                                hole::Hole::Descriptor.name);
      const char*       arguments[] =
        {
          "horizon",
          "-s",

          // XXX
          "-s",

          //
          // this option does not register FUSE as a daemon but
          // run it in foreground.
          //
          "-f",

          //
          // this option indicates the name of the file system type.
          //
          "-osubtype=infinit",

          //
          // this option disables remote file locking.
          //
          "-ono_remote_lock",

          //
          // this option indicates the kernel to perform reads
          // through large chunks.
          //
          "-olarge_read",

          //
          // this option indicates the kernel to perform writes
          // through big writes.
          //
          "-obig_writes",

          //
          // this option activates the in-kernel caching based on
          // the modification times.
          //
          "-oauto_cache",

          //
          // this option indicates the kernel to always forward the I/Os
          // to the filesystem.
          //
          "-odirect_io",

          //
          // this option specifies the name of the file system instance.
          //
          ofsname.c_str(),

          //
          // and finally, the mountpoint.
          //
          Infinit::Mountpoint.c_str()
        };

      struct ::fuse_operations          operations;
      {
        // set all the pointers to zero.
        ::memset(&operations, 0x0, sizeof (::fuse_operations));

        // Fill fuse functions array.
#define INFINIT_FUSE_LINK(Name) operations.Name = Name
#define INFINIT_FUSE_LINK_(R, Data, Elem)                       \
        INFINIT_FUSE_LINK(BOOST_PP_TUPLE_ELEM(2, 0, Elem));
        BOOST_PP_SEQ_FOR_EACH(INFINIT_FUSE_LINK_, _, INFINIT_FUSE_COMMANDS)
#undef INFINIT_FUSE_LINK_
#undef INFINIT_FUSE_LINK

        // the following flag being activated prevents the path argument
        // to be passed for functions which take a file descriptor.
        operations.flag_nullpath_ok = 1;
      }

      char* mountpoint;
      int multithreaded;

      if ((FUker::FUSE = ::fuse_setup(
             sizeof (arguments) / sizeof (elle::Character*),
             const_cast<char**>(arguments),
             &operations,
             sizeof (operations),
             &mountpoint,
             &multithreaded,
             NULL)) == NULL)
        goto _error;

      if (multithreaded)
        {
          if (::fuse_loop_mt(FUker::FUSE) == -1)
            goto _error;
        }
      else
        {
          if (::fuse_loop(FUker::FUSE) == -1)
            goto _error;
        }

      ::fuse_teardown(FUker::FUSE, mountpoint);

      // reset the FUSE structure pointer.
      FUker::FUSE = nullptr;

      // now that FUSE has stopped, make sure the program is exiting.
      elle::concurrency::Program::Exit();

      return NULL;

    _error:
      // log the error.
      log("%s", ::strerror(errno));

      // now that FUSE has stopped, make sure the program is exiting.
      elle::concurrency::Program::Exit();

      return (NULL);
    }

    ///
    /// XXX[to replace by the new signal mechanism]
    ///
    elle::Status        FUker::Run()
    {
      // create the FUSE-specific thread.
      if (::pthread_create(&FUker::Thread, NULL, &FUker::Setup, NULL) != 0)
        escape("unable to create the FUSE-specific thread");

      // XXX[race conditions exist here:
      //     1) the FUSE thread calls Program::Exit() before our event loop
      //        is entered.
      //     2) using the FUker::FUSE pointer to know if FUSE has been cleaned
      //        is a bad idea since teardown() could have been called, still
      //        the pointer would not be NULL. there does not seem to be much
      //        to do since we do not control FUSE internal loop and logic.]

      return elle::Status::Ok;
    }

    ///
    /// this method initializes the FUker by allocating a broker
    /// for handling the posted events along with creating a specific
    /// thread for FUSE.
    ///
    elle::Status        FUker::Initialize()
    {
      // XXX[to replace by the new signal mechanism]
      switch (hole::Hole::state)
        {
        case hole::Hole::StateOffline:
          {
            if (hole::Hole::ready.Subscribe(
                  elle::concurrency::Callback<>::Infer(&FUker::Run)) == elle::Status::Error)
              escape("unable to subscribe to the signal");

            break;
          }
        case hole::Hole::StateOnline:
          {
            if (FUker::Run() == elle::Status::Error)
              escape("unable to run the FUker thread");

            break;
          }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the FUker by making sure FUSE exits.
    ///
    elle::Status        FUker::Clean()
    {
      if (FUker::FUSE != nullptr)
        {
          // exit FUSE.
          ::fuse_exit(FUker::FUSE);

          // manually perform a file system call so as to wake up the FUSE
          // worker which is currently blocked waiting for data on the FUSE
          // socket. note that the call will not be treated as FUSE will
          // realise it has exited first.
          //
          // this is quite an un-pretty hack, but nothing has has been found
          // since the FUSE thread is blocked on an I/O.
          struct ::statfs stfs;
          ::statfs(Infinit::Mountpoint.c_str(), &stfs);

          // finally, wait for the FUSE-specific thread to exit.
          if (::pthread_join(FUker::Thread, NULL) != 0)
            log("%s", ::strerror(errno));
        }

      return elle::Status::Ok;
    }
  }
}
