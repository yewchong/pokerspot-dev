/*                thread_extensions.c                */

/*
  Lots of useful functions for all sorts of neat things.

   I changed the names to avoid all conflicts / confusion
   with Solaris threads.
*/

/*
cc  -G -o libthread_extensions.so thread_extensions.c -g -lpthread
        -lthread -lposix4

*/




#include <stdio.h>
#include <errno.h>
#include "thread_extensions.h"

pthread_key_t   sleep_queue_key, FIFO_MUTEX_QUEUE_KEY;

void dmutex_key_initializer();

#ifdef __sun
 /*  This runs before main() and declares the TSD key. */
#pragma init(dmutex_key_initializer)
#pragma init(fifo_key_initializer)
#endif

#if defined(__alpha) && (__osf__)
void __init_dmutex_key()
{
  dmutex_key_initializer();
  fifo_key_initializer();
}
#endif





/*
   ================================================================
                        Stringify (for printing) TIDs
   ================================================================
*/


typedef struct _pt
{pthread_t      tid;
 char           *name;
 struct _pt     *next;
} thread_name_t;

#if (!defined(__sun) && !defined(__sgi))

char *thread_name(pthread_t tid)
{char s[100];
 thread_name_t *n;
 static int tid_count = 1000;
 static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 static thread_name_t *thread_names = NULL;

 if (pthread_equal(NULL_TID, tid)) tid = pthread_self();
 pthread_mutex_lock(&lock);

   for (n = thread_names; n != NULL; n=n->next)
     {if (pthread_equal(n->tid, tid))
        goto L;
    }
 n = (thread_name_t *) malloc(sizeof(thread_name_t));
 n->tid = tid;
#if defined(__alpha) && defined(__osf__)
 sprintf(s, "T@%d", pthread_getsequence_np(tid));
#else
 sprintf(s, "T@%d", tid_count);
 tid_count++;
#endif
 n->name = (char *) malloc(strlen(s)+1);
 strcpy(n->name, s);
 n->next = thread_names;
 thread_names = n;

L:pthread_mutex_unlock(&lock);
 return(n->name);
}

#else
/* For known implementations, use the underlying TID numbering */

char *thread_name(pthread_t tid)
{char s[100];
 thread_name_t *n;

 static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 static thread_name_t *thread_names = NULL;

 if (pthread_equal(NULL_TID, tid)) tid = pthread_self();
 pthread_mutex_lock(&lock);

   for (n = thread_names; n != NULL; n=n->next)
     {if (pthread_equal(n->tid, tid))
        goto L;
    }
 n = (thread_name_t *) malloc(sizeof(thread_name_t));
 n->tid = tid;
 sprintf(s, "T@%d", (int) tid);
 n->name = (char *) malloc(strlen(s)+1);
 strcpy(n->name, s);
 n->next = thread_names;
 thread_names = n;

L:pthread_mutex_unlock(&lock);
 return(n->name);
}
#endif




/*
   ================================================================
                        Debug Mutex Lock
   ================================================================

        For debugging programs.  Do a global replace of _mutex_ with
        _dmutex_ and then the dmutex will detect self deadlocks, 
        non-owner unlocks, and record the owner's TID for use from
        the debugger.

        If you call pthread_dmutex_init(), then a macro will place
        the dmutex on a global list and record its name.  If you only
        use PTHREAD_DMUTEX_INITIALIZER, then no recording.  You may
        choose specific names via pthread_dmutex_init1().

        print_dmutexes() will print out the state (and sleep queue)
        for all mutexes on the list.  From the debugger:
        (debugger) call print_dmutexes()

*/

/* Debug mutexes defined out */
#if 0 
pthread_mutex_t dmutex_queue_lock = PTHREAD_MUTEX_INITIALIZER;
dmutex_queue_t  *dmutex_queue;


void dmutex_key_initializer()
{
  pthread_key_create(&sleep_queue_key, NULL);
 }

void insert_dmutex(pthread_dmutex_t *m)
{dmutex_queue_t *dq;

 dq = (dmutex_queue_t *) malloc(sizeof(dmutex_queue_t));
 dq->mutex = m;
 pthread_mutex_lock(&dmutex_queue_lock);
 dq->next = dmutex_queue;
 dmutex_queue = dq;
 pthread_mutex_unlock(&dmutex_queue_lock);
}

void remove_dmutex(dmutex_queue_t **q, pthread_dmutex_t *m)
{dmutex_queue_t *dq, *dq1;

 pthread_mutex_lock(&dmutex_queue_lock);

 if ((*q)->mutex == m)
   {dq1 = dmutex_queue->next;
    *q = dq1->next;
    free(dq1);
  }
 else
   for (dq = *q; dq->next != NULL; dq = dq->next)
     if (dq->next->mutex == m)
       {dq1 = dq->next;
        dq->next = dq1->next;
        free(dq1);
        break;
      }

 pthread_mutex_unlock(&dmutex_queue_lock);
}

void remove_sleeper(sleep_queue_t **q, pthread_t tid)
{sleep_queue_t *dq, *dq1;

 if (pthread_equal((*q)->tid, tid))
   {
     *q = (*q)->next;
   }
 else
   for (dq = *q; dq->next != NULL; dq = dq->next)
     if (pthread_equal(dq->next->tid, tid))
       {dq1 = dq->next;
        dq->next = dq1->next;
        break;
      }

}


void print_dmutex(pthread_dmutex_t *m)
{sleep_queue_t  *sq;

 pthread_mutex_lock(&(m->lock));
 flockfile (stdout); 
   if (!m->owned)
   printf("%s ---- Times locked: %3d,  failed: %3d.  Sleepers: ( ",
          m->name, m->n_locked, m->n_failed);
 else
   printf("%s %s  Times locked: %3d,  failed: %3d.  Sleepers: ( ",
          m->name, thread_name(m->owner), m->n_locked, m->n_failed);
 
 for (sq = m->sleepers; sq != NULL; sq = sq->next)
   printf("%s ", thread_name(sq->tid));
 printf(")\n");
 funlockfile (stdout); 
 pthread_mutex_unlock(&(m->lock));
}



void print_dmutexes()
{dmutex_queue_t *dq;
 pthread_dmutex_t *m;

 pthread_mutex_lock(&dmutex_queue_lock);
 for (dq = dmutex_queue; dq != NULL; dq = dq->next)
   {m = dq->mutex;
    print_dmutex(m);
  }
 pthread_mutex_unlock(&dmutex_queue_lock);
}

  

int pthread_dmutex_init1(pthread_dmutex_t *m, pthread_mutexattr_t *attr,
                         char *name)
{
  pthread_mutex_init(&(m->lock), NULL);
  pthread_cond_init(&(m->cv), NULL);
  m->owned = FALSE;
  m->owner = NULL_TID;
  m->name = name;
  m->sleepers = NULL;
  m->n_locked = 0;
  m->n_failed = 0;
  insert_dmutex(m);
}

int pthread_dmutex_destroy(pthread_dmutex_t *m)
{
  pthread_mutex_destroy(&(m->lock));
  pthread_cond_destroy(&(m->cv));
  remove_dmutex(&dmutex_queue, m);
}



int pthread_dmutex_lock(pthread_dmutex_t *m)
{pthread_t tid = pthread_self();
 sleep_queue_t *dq = (sleep_queue_t *) pthread_getspecific(sleep_queue_key);

 if (dq == NULL)
   {dq = (sleep_queue_t *) malloc(sizeof(sleep_queue_t));
    pthread_setspecific(sleep_queue_key, dq);
    dq->tid = tid;
  }

 pthread_mutex_lock(&(m->lock));
 if (pthread_equal(m->owner, tid))
   {if (m->name)
      printf("Error! %s deadlocking dmutex %s.\n", thread_name(tid), m->name);
   else
     printf("Error! %s deadlocking dmutex x%x.\n", thread_name(tid), m);
    abort();    /* I don't want it to exit.  I want it to crash. */
  }
 while (m->owned)
   {dq->next = m->sleepers;
    m->sleepers = dq;
    m->n_failed++;
    pthread_cond_wait(&(m->cv), &(m->lock));
    remove_sleeper(&(m->sleepers), tid);
   }
 m->owner = tid;
 m->owned = TRUE;
 m->n_locked++;
 pthread_mutex_unlock(&(m->lock));
 return(0);
}


int pthread_dmutex_trylock(pthread_dmutex_t *m)
{pthread_t tid = pthread_self();

 pthread_mutex_lock(&(m->lock));
 if (!(m->owned))
    {m->owner = tid;
     m->owned = TRUE;
     m->n_locked++;
     pthread_mutex_unlock(&(m->lock));
     return(0);
   }
 m->n_failed++;
 pthread_mutex_unlock(&(m->lock));
 return(EBUSY);
}


int pthread_dmutex_unlock(pthread_dmutex_t *m)
{pthread_t tid = pthread_self();

 pthread_mutex_lock(&(m->lock));
 if (!pthread_equal(m->owner, tid))
   {if (m->name)
      printf("Error! %s unlocking dmutex %s owned by %s\n",
             thread_name(tid), m->name, thread_name(m->owner));
   else
     printf("Error! %s unlocking dmutex x%x owned by %s\n",
            thread_name(tid), m, thread_name(m->owner));
    abort();    /* I don't want it to exit.  I want it to crash. */
    }
   
 m->owned = FALSE;
 m->owner = NULL_TID;
 pthread_mutex_unlock(&(m->lock));
 pthread_cond_signal(&(m->cv));
 return(0);
}
#endif



/*
   ================================================================
                        Timed Mutex Lock
   ================================================================

        Just like a timed CV, only for a mutex!
*/


int thread_timed_mutex_init(thread_timed_mutex_t *m,
                            pthread_mutexattr_t *attr)
{
  pthread_mutex_init(&(m->lock), NULL);
  pthread_cond_init(&(m->cv), NULL);
  m->owned = FALSE;
  m->owner = NULL_TID;
  m->abstime = 0;
  return 0;
}


int thread_timed_mutex_destroy(thread_timed_mutex_t *m)
{
  pthread_mutex_destroy(&(m->lock));
  pthread_cond_destroy(&(m->cv));
  return 0;
}



int thread_timed_mutex_lock(thread_timed_mutex_t *m,
                                const struct timespec *abstime)
{int err;
 pthread_t tid = pthread_self();

  pthread_mutex_lock(&(m->lock));
  while (m->owned)
    {err=pthread_cond_timedwait(&(m->cv), &(m->lock), abstime);
     if (err)
       {pthread_mutex_unlock(&(m->lock));
        return(err);
      }}
 m->owner = tid;
 m->owned = TRUE;
 pthread_mutex_unlock(&(m->lock));
 return(0);
}


int thread_timed_mutex_trylock(thread_timed_mutex_t *m)
{pthread_t tid = pthread_self();

 pthread_mutex_lock(&(m->lock));
 if (!m->owned)
    {m->owner = tid;
     m->owned = TRUE;
     pthread_mutex_unlock(&(m->lock));
     return(0);
   }
  pthread_mutex_unlock(&(m->lock));
  return(EBUSY);
}


int thread_timed_mutex_unlock(thread_timed_mutex_t *m)
{
  pthread_mutex_lock(&(m->lock));
  m->owned = FALSE;
  m->owner = NULL_TID;
  pthread_mutex_unlock(&(m->lock));
  pthread_cond_signal(&(m->cv));
  return 0;
}
