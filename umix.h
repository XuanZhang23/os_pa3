/*	Umix user-related major constants and interfaces
 *
 *	Relevant to user processes, i.e., pa[1-4]?.c in addition to kernel code.
 *
 *	The constants should NOT to be changed, unless explicitly said
 *	otherwise, as other already-compiled parts of the kernel rely on
 *	these constants.
 */

/*	Constants
 */

/*	Cars
 */
#define NUMPOS		10	/* max positions in road */
#define WEST		0	/* car direction: heading west */
#define EAST		1	/* car direction: heading east */

void Main ();			/* Main function in user program */

/*	System calls
 */

int Fork ();			/* pa1, pa2 */
int Yield (int p);		/* pa1 */
int Getpid ();			/* pa1 */
int Gettime ();			/* pa2 */
int RequestCPUrate (int m, int n);	/* pa2 */
int Regshm (char *a, int n);	/* pa3 */
int Seminit (int v);		/* pa3 */
int Wait (int s);		/* pa3 */
int Signal (int s);		/* pa3 */
int EnterRoad (int dir);	/* pa3 */
int ProceedRoad ();		/* pa3 */
int PrintRoad ();		/* pa3 */
int Atexit ();			/* pa4 */
int Exit ();			/* pa1, pa2 */
