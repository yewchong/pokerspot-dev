#ifndef __pthread_extensions_h__
#define __pthread_extensions_h__

/*
        A set of useful functions and macros for pthreads programs.
*/

#include <pthread.h>
#include <semaphore.h>

/* Some useful constants */
#ifndef NULL
# define NULL           ((void*)0)
#endif
#ifndef TRUE
# define TRUE           1
#endif
#ifndef FALSE
# define FALSE          0
#endif
#define DEBUG(arg)      {if (getenv("DEBUG")) arg;}
/* This NULL_TID may not work on all implementations! */
#define NULL_TID        (pthread_t) 0L



/*
   ================================================================
                        Debug Mutex Lock
   ================================================================
*/


#define PTHREAD_DMUTEX_INITIALIZER \
 {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, \
    NULL, NULL, 0, 0, FALSE, NULL_TID}

typedef struct _sleep_queue_t {
  pthread_t                     tid;
  struct _sleep_queue_t         *next;
} sleep_queue_t;


typedef struct _dmutex {
  pthread_mutex_t       lock;           /* lock for structure   */
  pthread_cond_t        cv;             /* waiting list control */
  char                  *name;          /* The declared name    */
  sleep_queue_t         *sleepers;      /* List of sleepers     */
  int                   n_locked;       /* Number of times locked */
  int                   n_failed;       /* Times woken w/o locking */
  int                   owned;          /* TRUE | FALSE         */
  pthread_t             owner;          /* NULL_TID or TID      */
} pthread_dmutex_t;

typedef struct _dmutex_queue_t {
  struct _dmutex                *mutex;
  struct _dmutex_queue_t        *next;
} dmutex_queue_t;

extern  int     pthread_dmutex_init(pthread_dmutex_t *, pthread_mutexattr_t *);
extern  int     pthread_dmutex_destroy(pthread_dmutex_t *);
extern  int     pthread_dmutex_lock(pthread_dmutex_t *);
extern  int     pthread_dmutex_trylock(pthread_dmutex_t *);
extern  int     pthread_dmutex_unlock(pthread_dmutex_t *);

#define pthread_dmutex_init(m, a) pthread_dmutex_init1(m, a, #m)


/*
   ================================================================
                        Timed Mutex Lock
   ================================================================
*/

#define THREAD_TIMED_MUTEX_INITIALIZER  \
 {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, FALSE, NULL_TID, 0}


typedef struct _timed_mutex {
  pthread_mutex_t       lock;           /* lock for structure           */
  pthread_cond_t        cv;             /* waiting list control         */
  int                   owned;          /* TRUE | FALSE                 */
  pthread_t             owner;          /* NULL_TID or TID              */
  const struct timespec *abstime;       /* absolute time before timeout */
} thread_timed_mutex_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern  int     thread_timed_mutex_init(thread_timed_mutex_t *,
                                        pthread_mutexattr_t *);
extern  int     thread_timed_mutex_destroy(thread_timed_mutex_t *);
extern  int     thread_timed_mutex_lock(thread_timed_mutex_t *,
                                        const struct timespec *);
extern  int     thread_timed_mutex_trylock(thread_timed_mutex_t *);
extern  int     thread_timed_mutex_unlock(thread_timed_mutex_t *);

#if defined(__cplusplus)
}
#endif


#endif