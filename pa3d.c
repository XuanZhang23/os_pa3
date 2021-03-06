/* Programming Assignment 3: Exercise D
 *
 * Now that you have a working implementation of semaphores, you can
 * implement a more sophisticated synchronization scheme for the car
 * simulation.
 *
 * Study the program below.  Car 1 begins driving over the road, entering
 * from the East at 40 mph.  After 900 seconds, both Car 3 and Car 4 try to
 * enter the road.  Car 1 may or may not have exited by this time (it should
 * exit after 900 seconds, but recall that the times in the simulation are
 * approximate).  If Car 1 has not exited and Car 4 enters, they will crash
 * (why?).  If Car 1 has exited, Car 3 and Car 4 will be able to enter the
 * road, but since they enter from opposite directions, they will eventually
 * crash.  Finally, after 1200 seconds, Car 2 enters the road from the West,
 * and traveling twice as fast as Car 4.  If Car 3 was not coming from the
 * opposite direction, Car 2 would eventually reach Car 4 from the back and
 * crash.  (You may wish to experiment with reducing the initial delay of
 * Car 2, or increase the initial delay of Car 3, to cause Car 2 to crash
 * with Car 4 before Car 3 crashes with Car 4.)
 *
 * Exercises
 *
 * 1. Modify the procedure DriveRoad such that the following rules are obeyed:
 *
 *	A. Avoid all collisions.
 *
 *	B. Multiple cars should be allowed on the road, as long as they are
 *	traveling in the same direction.
 *
 *	C. If a car arrives and there are already other cars traveling in the
 *	SAME DIRECTION, the arriving car should be allowed enter as soon as it
 *	can. Two situations might prevent this car from entering immediately:
 *	(1) there is a car immediately in front of it (going in the same
 *	direction), and if it enters it will crash (which would break rule A);
 *	(2) one or more cars have arrived at the other end to travel in the
 *	opposite direction and are waiting for the current cars on the road
 *	to exit, which is covered by the next rule.
 *
 *	D. If a car arrives and there are already other cars traveling in the
 *	OPPOSITE DIRECTION, the arriving car must wait until all these other
 *	cars complete their course over the road and exit.  It should only wait
 *	for the cars already on the road to exit; no new cars traveling in the
 *	same direction as the existing ones should be allowed to enter.
 *
 *	E. This last rule implies that if there are multiple cars at each end
 *	waiting to enter the road, each side will take turns in allowing one
 *	car to enter and exit.  (However, if there are no cars waiting at one
 *	end, then as cars arrive at the other end, they should be allowed to
 *	enter the road immediately.)
 *	
 *	F. If the road is free (no cars), then any car attempting to enter
 *	should not be prevented from doing so.
 *
 *	G. All starvation must be avoided.  For example, any car that is
 *	waiting must eventually be allowed to proceed.
 *
 * This must be achieved ONLY by adding synchronization and making use of
 * shared memory (as described in Exercise C).  You should NOT modify the
 * delays or speeds to solve the problem.  In other words, the delays and
 * speeds are givens, and your goal is to enforce the above rules by making
 * use of only semaphores and shared memory.
 *
 * 2. Devise different tests (using different numbers of cars, speeds,
 * directions) to see whether your improved implementation of DriveRoad
 * obeys the rules above.
 *
 * IMPLEMENTATION GUIDELINES
 * 
 * 1. Avoid busy waiting. In class one of the reasons given for using
 * semaphores was to avoid busy waiting in user code and limit it to
 * minimal use in certain parts of the kernel. This is because busy
 * waiting uses up CPU time, but a blocked process does not. You have
 * semaphores available to implement the driveRoad function, so you
 * should not use busy waiting anywhere.
 *
 * 2. Prevent race conditions. One reason for using semaphores is to
 * enforce mutual exclusion on critical sections to avoid race conditions.
 * You will be using shared memory in your driveRoad implementation.
 * Identify the places in your code where there may be race conditions
 * (the result of a computation on shared memory depends on the order
 * that processes execute).  Prevent these race conditions from occurring
 * by using semaphores.
 *
 * 3. Implement semaphores fully and robustly.  It is possible for your
 * driveRoad function to work with an incorrect implementation of
 * semaphores, because controlling cars does not exercise every use of
 * semaphores.  You will be penalized if your semaphores are not correctly
 * implemented, even if your driveRoad works.
 *
 * 4. Avoid starvation.  This is especially relevant when implementing the
 * Signal function.  If there are multiple processes that blocked on the
 * same semaphore, then a good policy is to unblock them in FIFO order.
 *
 * 5. Control cars with semaphores: Semaphores should be the basic
 * mechanism for enforcing the rules on driving cars. You should not
 * force cars to delay in other ways inside driveRoad such as by calling
 * the Delay function or changing the speed of a car. (You can leave in
 * the delay that is already there that represents the car's speed, just
 * don't add any additional delaying).  Also, you should not be making
 * decisions on what cars do using calculations based on car speed (since
 * there are a number of unpredictable factors that can affect the
 * actual cars' progress).
 *
 * GRADING INFORMATION
 *
 * 1. Semaphores: We will run a number of programs that test your
 * semaphores directly (without using cars at all). For example:
 * enforcing mututal exclusion, testing robustness of your list of
 * waiting processes, calling signal and wait many times to make sure
 * the semaphore state is consistent, etc.
 *
 * 2. Cars: We will run some tests of cars arriving in different ways,
 * to make sure that you correctly enforce all the rules for cars given
 * in the assignment.  We will use a correct semaphore implementation for
 * these tests so that even if your semaphores are not correct you could
 * still get full credit on the driving part of the grade.  Think about
 * how your driveRoad might handle different situations and write your
 * own tests with cars arriving in different ways to make sure you handle
 * all cases correctly.
 *
 *
 * WHAT TO TURN IN
 *
 * You must turn in two files: mykernel3.c and p3d.c.  mykernel3.c should
 * contain you implementation of semaphores, and p3d.c should contain
 * your modified version of InitRoad and DriveRoad (Main will be ignored).
 * Note that you may set up your static shared memory struct and other
 * functions as you wish. They should be accessed via InitRoad and DriveRoad,
 * as those are the functions that we will call to test your code.
 *
 * Your programs will be tested with various Main programs that will exercise
 * your semaphore implementation, AND different numbers of cars, directions,
 * and speeds, to exercise your DriveRoad function.  Our Main programs will
 * first call InitRoad before calling DriveRoad.  Make sure you do as much
 * rigorous testing yourself to be sure your implementations are robust.
 */

#include <stdio.h>
#include "aux.h"
#include "umix.h"

#define CAR1	1
#define CAR2	2
#define CAR3	3
#define CAR4	4
#define MAXPROCS 25

#define NONE  2
#define DEBUG 0
#define DEBUG1 0   //labels 


typedef int semaphore;
typedef int queue;
typedef struct {
  semaphore sem;
  int car;
  int from;
} position;

static struct {
  position pos[NUMPOS+1];
  semaphore mutex;        
  semaphore westGate;                     // waiting to go east
  semaphore eastGate;                     // waiting to go west
  int westbound;
  int eastbound;
  int westWait;
  int eastWait;

  queue waitlist[MAXPROCS];                   // direction order of waiting processes
  int numWait;
  int whead;
  int wtail;
}shm;

void InitRoad ();
void driveRoad (int c, int from, int mph);

int getwestbound();          // going west on the road
int geteastbound();          // going east on the raod

void entryWest();
void exitWest();
void entryEast();
void exitEast();

void Arrive (int from, int c);
void Depart (int from, int c);

/* testing functions*/
void printDirection();
void printRoad (int num);
void printWaitlist();
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

 if (Fork () == 0) {
       Delay (0);
           driveRoad (2, WEST, 10);
               Exit ();
                 }

   if (Fork () == 0) {
         Delay (0);
             driveRoad (3, WEST, 20);
                 Exit ();
                   }

     if (Fork () == 0) {
           Delay (0);
               driveRoad (4, WEST, 30);
                   Exit ();
                     }

       if (Fork () == 0) {
             Delay (0);
                 driveRoad (5, WEST, 40);
                     Exit ();
                       }

         if (Fork () == 0) {
               Delay (0);
                   driveRoad (6, EAST, 50);
                       Exit ();
                         }

           if (Fork () == 0) {
                 Delay (0);
                     driveRoad (7, EAST, 60);
                         Exit ();
                           }

             if (Fork () == 0) {
                   Delay (0);
                       driveRoad (8, EAST, 70);
                           Exit ();
                             }

               if (Fork () == 0) {
                     Delay (0);
                         driveRoad (9, EAST, 80);
                             Exit ();
                               }

                 if (Fork () == 0) {
                       Delay (0);
                           driveRoad (10, EAST, 90);
                               Exit ();
                                 }

                   driveRoad (1, WEST, 5);
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
  Regshm ((char *) &shm, sizeof (shm));
  for(int i = 0; i < NUMPOS+1 ; i++) {
    shm.pos[i].sem = Seminit (1);
    shm.pos[i].car = 0;
    shm.pos[i].from = NONE;  // no direction
    shm.waitlist[i] = NONE;
  }
  shm.mutex     = Seminit (1);    // mutex for shared vars
  shm.westGate    = Seminit (0);    // cars don't wait if road is empty
  shm.eastGate    = Seminit (0);    //     signals only when car(s) wait
  shm.eastbound = 0;              // no on on the road
  shm.westbound = 0;              // no on on the road
  shm.eastWait  = 0;              // no on is waiting
  shm.westWait  = 0;              // no on is waiting
  shm.numWait = 0;
  shm.whead = 0;
  shm.wtail = 0;
}

#define IPOS(FROM)	(((FROM) == WEST) ? 1 : NUMPOS)

void driveRoad (c, from, mph)
	int c, from, mph;
{
	int p, np, i;				/* positions */
  
  p = 0;
  np = IPOS(from);
  

  Wait (shm.mutex);
  Arrive (from, c);
  Signal (shm.mutex);

  if (DEBUG1) {
    Printf ("Car %d arrives from %d                          *****\n", c, IPOS(from));
  }
  
  Wait (shm.pos[np].sem);
  EnterRoad (from);
  shm.pos[IPOS(from)].car = Getpid ();
  shm.pos[IPOS(from)].from =  from;
  Signal (shm.pos[p].sem);

  PrintRoad ();
	Printf ("Car %d enters at %d at %d mph\n", c, IPOS(from), mph);
  

  /* print road map at enter */
  if (DEBUG) { 
    Printf ("                                              #\n");
    printRoad (0);
    Printf ("\n\n");
  }

  /* go through all positions */
	for (i = 1; i < NUMPOS; i++) {
		if (from == WEST) {
			p = i;                           // car's current position 
			np = i + 1;                      // car's next position 
		} else {
			p = NUMPOS + 1 - i;
			np = p - 1;
		}
		Delay (3600/mph);
    
    if(DEBUG) {              
      printRoad (0);                   // print pre-cs
    }
    
    Wait (shm.pos[np].sem);              // CRITICAL SECTION (starts)

    if(shm.pos[np].car == 0) {             // you are in p, you want to enter np
		  ProceedRoad ();
      shm.pos[np].car = Getpid ();         // you have entered in shm.position np, you have left p 
      shm.pos[np].from = from;

      shm.pos[p].car = 0;                  // reset prev shm.pos values
      shm.pos[p].from = NONE;
      PrintRoad ();
      Printf ("Car %d moves from %d to %d \n", c, p, np);
      
      if(DEBUG1) printWaitlist ();
    } else {
      /* error if this prints */
      Printf ("Car %d doesn't move from %d to %d              X\n", c, p, np);
    }

		//PrintRoad ();
    Signal (shm.pos[p].sem);             // CRITICAL SECTION (ends)

    
    if(DEBUG) {             
      Printf ("\n");
      printRoad (1);                   // print post-cs
      Printf ("\n");
      printDirection();
      Printf ("\n"); 
    }
	}

	Delay (3600/mph);

  if (DEBUG) {
    Printf ("\n2nd CS - start\n");
    printRoad (0);
  }
                                       // CRITICAL SECTION
  ProceedRoad (); 
  shm.pos[np].car = 0;                     // car exits -> reset vals 
  shm.pos[np].from = NONE;
  Signal (shm.pos[np].sem);
  
 /* if (from == WEST) {
    exitWest();
  } else { exitEast(); } 
*/

  Wait (shm.mutex);
  Depart (from, c);
  Signal (shm.mutex);

  if (DEBUG) {
    printRoad (1);
    Printf ("2nd CS - end \n");
  }

  
	PrintRoad ();
	Printf ("Car %d exits road                          ->>>\n", c);

  
  if (DEBUG) {
    printDirection();
    printRoad (2);
    Printf ("\n\n");
  }
}

void printRoad (int num) {
 
  Printf ("-------------------------------------------\n");
  if(num == 0) {
    Printf ("pre-cs, car %d\n", Getpid ()); 
  } else if(num == 1) {
    Printf ("post-cs, car %d\n", Getpid ()); 
  } else {
    Printf ("road after car %d exits\n", Getpid ());
  }

  for(int i = 0; i < NUMPOS+1; i++) {     
    Printf ("%d - ", i);                // print positions
  }
  Printf ("\n");
  
  for(int i =0 ; i < NUMPOS+1; i++) {     // print cars
    Printf ("%d   ", shm.pos[i].car);
  }
  Printf ("\n");
  
  for(int i = 0; i < NUMPOS+1; i++) {
    int v = shm.pos[i].from;
    char from = 'n';
    if (v == 0 && i !=0) {
      from = 'w'; 
    } else if (v == 1) { 
      from = 'e';
    } else if (v == 2) {
      from = '-';
    }
    Printf ("%c   ", from);
  }
  Printf ("\n");
  Printf ("-------------------------------------------\n");
}

int getwestbound() {
  int c = 0;
  for (int i = 1; i < NUMPOS+1; i++){
    if(shm.pos[i].from == 1) { c++; } 
  }
  return c;
}

int geteastbound() {
  int c = 0;
  for (int i = 1; i < NUMPOS+1; i++){
    if(shm.pos[i].from == 0) { c++; } 
  }
  return c;
}


void printDirection(){
  Printf ("\nfrom west: %d; east: %d \n", shm.eastbound, shm.westbound );

}

void Arrive (int from, int c) {
//  Wait (shm.mutex);
  // Printf ("from: %d\n", from);
  // if cars on the road, then...
  if ((getwestbound() > 0) || (geteastbound() > 0)) {
   
    // if car is coming from the west entrance...
    if (from == WEST) {
      // if someone is going westbound, car waits
      if ((getwestbound() > 0) || (shm.eastWait > 0) ) {
        shm.westWait++;                       // count waiters, and wait for signal
        // update direction order list
        shm.waitlist[shm.wtail] = from;       // double check 
        shm.wtail = (shm.wtail+1)%(MAXPROCS); // increment tail
        
        if(DEBUG1) {
          Printf ("Car %d on waitlist on side %d\n", c, from);
        }

        Signal (shm.mutex);     // release mutex before waiting
        Wait (shm.westGate);
        shm.westWait--;
      }
      
      if(DEBUG) {
        Printf ("Car %d is on eastbound\n", c);
      }
    }
    // if car is coming from the east entrance...
    else if (from == EAST) {
      if ((geteastbound() > 0) || (shm.westWait > 0)) {
        shm.eastWait++;                       // count waiters, and wait for signal        
        // update direction order list
        shm.waitlist[shm.wtail] = from;       // double check 
        shm.wtail = (shm.wtail+1)%(MAXPROCS); // increment tail

        if(DEBUG1) {
          Printf ("Car %d on waitlist on side %d\n", c, from);
        }

        Signal (shm.mutex);     // release mutex before waiting
        Wait (shm.eastGate);
        shm.eastWait--;
      }
  
      if(DEBUG) {
        Printf ("Car %d is on westbound bound\n", c);
      }
  } 
  


  } 
  // count how many cars are on the road
   if (from == WEST) shm.eastbound++;
   else shm.westbound++;
//  Signal (shm.mutex);
}

void Depart (int from, int c) {
 // Wait (shm.mutex);
  
  if(from == WEST) {
    shm.eastbound--;
  } else {
    shm.westbound--;
  }

  int dir = shm.waitlist[shm.whead];      // get next dir from wait queue
  if (dir == WEST ) { 
    if ((getwestbound() == 0) || geteastbound() > 0){
      shm.whead = (shm.whead+1)%(MAXPROCS);
      Signal (shm.westGate);
      if (DEBUG1) Printf ("Gate %d is open \n", dir);
    } else {
      if(DEBUG1) Printf ("Gate %d can't open, oncoming traffic exists!\n", dir);
    }
  }
 if (dir == EAST) {
    if((geteastbound() == 0) || getwestbound() > 0) {
      shm.whead = (shm.whead+1)%(MAXPROCS);
      Signal (shm.eastGate);
      if (DEBUG1) Printf ("Gate %d is open \n", dir);
    }
   else {
      if(DEBUG1) Printf ("Gate %d can't open, oncoming traffic exists!\n", dir);
    }
 }
 if (dir == NONE) {
 //   shm.whead = (shm.whead+1)%(MAXPROCS);
 }
  //Signal (shm.mutex);
}

void printWaitlist () {
  Printf ("[");
  for( int i = shm.whead; i < shm.wtail+1; i++){
    Printf ("%d; ", shm.waitlist[i]);
  }
  Printf ("]\n");
}
