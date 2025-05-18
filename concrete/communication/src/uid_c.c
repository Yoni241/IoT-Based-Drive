#include <stddef.h>  /* for size_t      */
#include <time.h>    /* for time        */
#include <unistd.h>  /* for pid         */
#include <pthread.h> /* for thread safe */

#include "uid_c.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

ilrd_uid_t UIDCreate(void)
{
	static size_t count = 0;
	struct ilrd_uid uid;
	
	pthread_mutex_lock(&mutex);

		uid.time_stamp = time(NULL);
		uid.pid_stamp = getpid(); 
		++count;
		uid.counter = count;

	pthread_mutex_unlock(&mutex);

	return uid;
}

int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2)
{
	return (uid1.time_stamp == uid2.time_stamp) &&
	 	   (uid1.pid_stamp == uid2.pid_stamp) &&
	 	   (uid1.counter == uid2.counter);
}

ilrd_uid_t UIDBadUID(void)
{
	ilrd_uid_t bad_uid = {(time_t) -1, (pid_t) 0, 0};
	
	return bad_uid;
}

