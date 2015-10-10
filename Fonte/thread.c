/* You can compile this program with:
 * gcc -Wall -D_REENTRANT -o thread thread.c<\n>
 * -lpthread */
/* We always need to include this header file for<\n>
 * the threads */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
/* This is the prototype for our thread function */
void *mythread(void *data);
/* We must initialize our mutex */
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
/* This will be a shared variable between all of<\n>
 * the threads */
int x = 0;
int main(void) {
   /* This will be an array holding the thread ids<\n>
    * for each thread */
   /* We keep track of all the tids so that we can<\n>
    * call pthread_join()later to retrieve
    * the return value from the thread */
   pthread_t tids[10];
   int i;
   /* We will now create the 10 threads. Each<\n>
    * thread will increment x until x is 4000.
    * On our last argument ot pthread_create
    * we could have passed an argument to the
    * thread function */
   for(i=0; i<10; i++) {
      pthread_create(&tids[i], NULL, mythread, NULL);
   }
   /* We will now wait for each thread to<\n>
    * terminate */
   for(i=0; i<10; i++) {
   /* This will block until the specified
    * thread finishes execution. Our second
    * argument to pthread_join can be a pointer
    * that will have the return value of the
    * thread stored in it */
      pthread_join(tids[i], NULL);
      printf("Thread id %ld returned\n", tids[i]);
   }
   return(1);
}
   /* This is our actual thread function */
void *mythread(void *data) {
struct timeval tv;
struct timezone tz;
struct tm *tm;
   while(x < 100) {
      /* We will now try to lock the mutex. If
       * another thread already has it locked, we
       * block until it is available again. After
       * you first run this program, you should
       * comment out the lock/unlock lines in this
       * function so that you can see why you need
       * mutexes. */
      pthread_mutex_lock(&count_mutex);
      x++;
      /* We will have it print out the thread ID
       * and the value of X */
      gettimeofday(&tv, &tz);
      tm=localtime(&tv.tv_sec);
      printf("Thread ID%ld: X is now %d (%d:%02d:%02d:%d).\n",
      pthread_self(), x, tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);
      /* We will now release the mutex so that
       * another thread gets the chance to run. */
      pthread_mutex_unlock(&count_mutex);
     usleep(100);
   }
   /* We can return a pointer. Whatever pointer
    * we return can later be retrieved using the
    * pthread_join function */
   pthread_exit(NULL);
}

