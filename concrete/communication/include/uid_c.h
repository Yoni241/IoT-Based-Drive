#ifndef __UID_C_H__
#define __UID_C_H__

#include <sys/types.h>

typedef struct ilrd_uid ilrd_uid_t;

struct ilrd_uid
{
	time_t time_stamp;
	pid_t pid_stamp;
	size_t counter;
};

/*
	@ Description: creates a UID.
	@ Params: none.
	@ Return value: a UID of type ilrd_uid_t.
	@ Errors: if create fails, returns the element BadUID. 
	@ Time complexity: O(1).
    @ Space complexity: O(1).
*/
ilrd_uid_t UIDCreate(void);

/*
	@ Description: checks whether two UIDs are the same.
	@ Params: uid1 - the first uid to be compared.
			  uid2 - the second uid to be compared.
	@ Return value: 1 if same, 0 otherwise.
	@ Errors: none.
	@ Time complexity: O(1).
    @ Space complexity: O(1).
*/
int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2);

/*
	@ Description: creates an invalid UID element - the BadUID element.
	@ Params: none.
	@ Return value: the BadUID element.
	@ Errors: none.
	@ Time complexity: O(1).
    @ Space complexity: O(1).
*/
ilrd_uid_t UIDBadUID(void);

#endif /* __UID_C_H__ */
