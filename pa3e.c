#include <stdio.h>
#include "aux.h"
#include "umix.h"

#define CAR1	1
#define CAR2	2
#define CAR3	3
#define CAR4	4

void InitRoad ();
void driveRoad (int c, int from, int mph);

void Main ()
{
	InitRoad ();

	/* The following code is specific to this particular simulation,
	 * e.g., number of cars, directions, and speeds.  You should
	 * experiment with different numbers of cars, directions, and
	 * speeds to test your modification of DriveRoad.  When your
	 * solution is tested, we will use different Main procedures,
	 * which will first call InitRoad before any calls to DriveRoad.
	 * So, you should do any initializations in InitRoad.
	 */

	if (Fork () == 0) {				/* Car 2 */
		Delay (200);
		driveRoad (CAR2, WEST, 70);
		Exit ();
	}


	driveRoad (CAR1, WEST, 40);			/* Car 1 */

	Exit ();
}

/* Our tests will call your versions of InitRoad and DriveRoad, so your
 * solution to the car simulation should be limited to modifying the code
 * below.  This is in addition to your implementation of semaphores
 * contained in mykernel3.c.
 */

void InitRoad ()
{
	/* do any initializations here */
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
