/* mykernel.c: your portion of the kernel
 *
 *	Below are procedures that are called by other parts of the kernel.
 *	Your ability to modify the kernel is via these procedures.  You may
 *	modify the bodies of these procedures any way you wish (however,
 *	you cannot change the interfaces).
 */

#include "aux.h"
#include "sys.h"
#include "mykernel3.h"

#define FALSE 0
#define TRUE 1
#define DEBUG 0
#define DEBUG2 0
/*	A sample semaphore table.  You may change this any way you wish.
 */
typedef int queue;

static struct {
	int valid;	/* Is this a valid entry (was sem allocated)? */
	int value;	/* value of semaphore */
  queue bq[MAXSEMS];
  int bqHead;
  int bqTail;
} semtab[MAXSEMS];


//static struct {   /* waiting/blocked processes queue */
//  int pid;
//} bq[MAXSEMS];

// int bqHead;       /* pointer to bq head */
// int bqTail;

/*	InitSem () is called when kernel starts up.  Initialize data
 *	structures (such as the semaphore table) and call any initialization
 *	procedures here.
 */

void printQ();
void InitSem ()
{
	int s;

	/* modify or add code any way you wish */
	for (s = 0; s < MAXSEMS; s++) {		/* mark all sems free */
		semtab[s].valid = FALSE;
    for (int t = 0; t< MAXSEMS; t++){
      semtab[s].bq[t] = 0;
    }
	}
}

/*	MySeminit (p, v) is called by the kernel whenever the system
 *	call Seminit (v) is called.  The kernel passes the initial
 * 	value v, along with the process ID p of the process that called
 *	Seminit.  MySeminit should allocate a semaphore (find a free entry
 *	in semtab and allocate), initialize that semaphore's value to v,
 *	and then return the ID (i.e., index of the allocated entry).
 */

int MySeminit (p, v)
	int p, v;
{
	int s;

	/* modify or add code any way you wish */
	for (s = 0; s < MAXSEMS; s++) {
		if (semtab[s].valid == FALSE) {
			break;
		}
	}
	if (s == MAXSEMS) {
		Printf ("No free semaphores\n");
		return (-1);
	}

	semtab[s].valid = TRUE;
	semtab[s].value = v;

	return (s);
}

/*	MyWait (p, s) is called by the kernel whenever the system call
 *	Wait (s) is called.
 */

void MyWait (p, s)
	int p, s;
{
	/* modify or add code any way you wish */
	semtab[s].value--;
  if (DEBUG) Printf ("car %d calls wait to sem %d \n", p, s);
  int i;
  if(semtab[s].value < 0) {
     int tail = semtab[s].bqTail;
     semtab[s].bq[tail] = p;    // putting process into waiting queue.
     semtab[s].bqTail = (tail+1)%MAXSEMS;
    if (DEBUG) Printf ("---> car %d blocked from sem %d \n", p,s);
     Block (p);
  }
 if (DEBUG2)  printQ();
}

/*	MySignal (p, s) is called by the kernel whenever the system call
 *	Signal (s) is called.
 */

void MySignal (p, s)
	int p, s;
{ 
  int preval = semtab[s].value;
	/* modify or add code any way you wish */
	semtab[s].value++;
  if (DEBUG) Printf ("\ncar %d calls signal to sem %d", p,s);

  if(semtab[s].value <= 0) {
    int head = semtab[s].bqHead;
    int nextProc = semtab[s].bq[head];         /* get next waiting process */
    semtab[s].bq[head] = 0;
    semtab[s].bqHead = (1 + head) % MAXSEMS;        /* increment pointer */
    if (DEBUG) Printf (" --> car %d unblocked from sem %d \n", nextProc, s);

    Unblock (nextProc);
  }
 if(DEBUG2) printQ();
}

void printQ(){
  Printf ("\n[");
  for(int i = 0; i < MAXSEMS; i++) {
    for(int j = 0; j < MAXSEMS; j++){
    Printf ("%d; ", semtab[i].bq[j]);
    }
  Printf ("] head:%d tail:%d \n", semtab[i].bqHead, semtab[i].bqTail);
  }
}
