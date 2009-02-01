#ifndef THREADSUPPORT_H_
#define THREADSUPPORT_H_


#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

#pragma comment(lib, "boost_thread-vc80-mt-1_35.lib")

extern boost::mutex io_mutex;
typedef boost::mutex::scoped_lock scoped_lock;

#define IO_LOCK scoped_lock lock(io_mutex)

/* Use IO_LOCK; ONLY in scoped occurance around cout calls, 
   where there is ABSOLUTELY NO CHANCE IN HELL that another function 
   could be called that could also try to lock the io_mutex,
   that would result in deadlock!
*/



#endif