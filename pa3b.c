/* Programming Assignment 3: Exercise B
 *
 * One way to solve the "race condition" causing the cars to crash is to add
 * synchronization directives that cause cars to wait for others.  In this
 * assignment, we will use semaphores.  The kernel supports a large number
 * of semaphores (defined by MAXSEMS in umix.h, currently set to 100), and
 * each semaphore is identified by an integer 0 - 99 (MAXSEMS-1).  Semaphore
 * operations are:
 *
 *	int s = Seminit (int v)
 *
 *		Allocates a semaphore and initializes its value to v.
 *		Returns a unique identifier s of the semaphore, which is
 *		then used to refer to the semaphore in Wait and Signal
 *		operations.  Returns -1 if unsuccessful (e.g., if there
 *		are no available semaphores).
 *
 *	Wait (int s)
 *
 *		Semaphore wait operation.
 *
 * 	Signal (int s)
 *
 *		Semaphore signal operation.
 *
 * The above are system calls that can be called by user programs.  These
 * procedures cause a trap into the kernel, and each calls a corresponding
 * procedure located in mykernel3.c:
 *
 * 	int s = MySeminit (int p, int v)
 *
 *	MyWait (int p, int s)
 *
 *	MySignal (int p, int s)
 *
 * Notice that these routines take an additional parameter p, which is the
 * process ID of the calling process.  Given these interfaces, you are to
 * implement these routines.
 *
 * Exercises
 *
 * 1. Study the program below.  Process 1 (Car 1) allocates a semaphore,
 * storing its ID in sem, and initializes its value to 0.  It then creates
 * process 2 (Car 2) which immediately executes Wait (sem).  What should
 * happen to process 2 given that sem is initialized to 0?  After driving
 * over the road, process 1 executes Signal (sem).  What should happen to
 * process 2?
 *
 * 2. Study the file mykernel3.c.  It contains a skeletal data structure and
 * code for the semaphore operations.  Notice how MySeminit finds a free 
 * entry in the semaphore table, allocates it, initializes it, and uses
 * the index as the semaphore ID that is returned.  The other routines,
 * MyWait and MySignal have minimal bodies that decrement and increment
 * the semaphore value, but have no effect on synchronization.  To
 * implement synchronization, you need two utility kernel functions
 * that are provided to you:
 *
 * 	Block (int p) causes process p to block.  It basically removes p
 *	from being eligible for scheduling, and context switches to another
 *	process that is eligible.
 *
 *	Unblock (int p) causes process p to be eligible for scheduling.
 *	This does not mean it will execute immediately, but only that
 *	when a scheduling decision is made, p may be selected.
 *
 * Given these utility routines, implement the semaphore routines.
 *
 * NOTE: The kernel already enforces atomicity of MySignal and MyWait, 
 * so you do NOT need implement any additional mechansims for atomicity.
 *
 * 3. Run the program below.  It should now cause Car 2 to wait for Car 1
 * before driving over the road, thus avoiding a crash.
 *
 */

#include <stdio.h>
#include "aux.h"
#include "umix.h"

#define CAR1	1
#define CAR2	2

void driveRoad (int c, int from, int mph);

void Main ()
{
	int sem;

	sem = Seminit (0);

	if (Fork () == 0) {				/* Car 2 */
		Wait (sem);
		Delay (0);
		driveRoad (CAR2, EAST, 60);
		Exit ();
	}

	driveRoad (CAR1, WEST, 40);		/* Car 1 */
	Signal (sem);

	Exit ();
}

#define IPOS(FROM)	(((FROM) == WEST) ? 1 : NUMPOS)

void driveRoad (c, from, mph)
	int c, from, mph;
{
	int p, np, i;				/* positions */

	EnterRoad (from);
	PrintRoad ();
	Printf ("Car %d enters at %d at %d mph\n", c, IPOS(from), mph);

	for (i = 1; i < NUMPOS; i++) {
		if (from == WEST) {
			p = i;
			np = i + 1;
		} else {
			p = NUMPOS + 1 - i;
			np = p - 1;
		}

		Delay (3600/mph);
		ProceedRoad ();
		PrintRoad ();
		Printf ("Car %d moves from %d to %d\n", c, p, np);
	}

	Delay (3600/mph);
	ProceedRoad ();
	PrintRoad ();
	Printf ("Car %d exits road\n", c);
}
