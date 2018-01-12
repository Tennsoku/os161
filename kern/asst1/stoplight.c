/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>


/*
 *
 * Constants
 *
 */

/*
 * Number of cars created.
 */

#define NCARS 20

/*
 *
 * Semaphores
 *
 */

// Block sem, Mutex
// Indexs are same as directions.
// Since indexs go anticlkwise but straight and left is 
// clkwise direction, the index should decrement.

struct semaphore *NW, *NE, *SE, *SW;
struct semaphore *Block_sem[4];

// Car entering blocks, Mutex
struct semaphore *entering_NW, *entering_NE, *entering_SE, *entering_SW;
struct semaphore *entering_sem[4];

// Cars In Intersection
struct semaphore *CII;

// Straight priority sems. Mutex
// When straight priority sem is held on one direction,
// the car in that direction cannot go straight.
// When a car is doing left turn or going straight, it 
// should hold the opposite direction priority when entering,
// and release after leaving the first block.

// struct semaphore *NS_straight, *EW_straight;
// struct semaphore *straight_priority[2];


/*
 *
 * Function Definitions
 *
 */

static const char *directions[] = { "N", "E", "S", "W" };
static const char *turns[] = { "left", "right" };

static const char *msgs[] = {
        "approaching:",
        "region1:    ",
        "region2:    ",
        "region3:    ",
        "leaving:    "
};

/* use these constants for the first parameter of message */
enum { APPROACHING, REGION1, REGION2, REGION3, LEAVING };

static void
message(int msg_nr, int carnumber, int cardirection, int destdirection)
{
        kprintf("%s car = %2d, direction = %s, destination = %s\n",
                msgs[msg_nr], carnumber,
                directions[cardirection], directions[destdirection]);
}
 
/*
 * gostraight()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement passing straight through the
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
gostraight(unsigned long cardirection,
           unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */
        
        (void) cardirection;
        (void) carnumber;


        //Destination direction
        int destination = (cardirection + 2) % 4;
        //kprintf("car %d is going straight.\n", carnumber);

        //Block info
        int region1 = (cardirection + 4) % 4;
        int region2 = (cardirection + 3) % 4;


        //Get entering semaphore
        P(entering_sem[cardirection]);              // Enter

        //Approaching
        message(APPROACHING, carnumber, cardirection, destination);

        //Region 1
        P(CII);                                     // Cars in intersection
        P(Block_sem[region1]);                      // Get into block

        message(REGION1, carnumber, cardirection, destination);
        V(entering_sem[cardirection]);              // Release entering sem   



        
        //Region 2
        P(Block_sem[region2]);                      // Get next block
        message(REGION2, carnumber, cardirection, destination);
        V(Block_sem[region1]);                      // Release privous block     

        //Leaving
        message(LEAVING, carnumber, cardirection, destination);
        
        V(Block_sem[region2]); 
        V(CII);




}


/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(unsigned long cardirection,
         unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;


        //Destination direction
        int destination = (cardirection+1) % 4;
        //kprintf("car %d is turning left.\n", carnumber);


        //Block info
        int region1 = (cardirection + 4) % 4;
        int region2 = (cardirection + 3) % 4;
        int region3 = (cardirection + 2) % 4;


        //Get entering semaphore
        P(entering_sem[cardirection]);              // Enter

        //Approaching
        message(APPROACHING, carnumber, cardirection, destination);

        //Region 1
        P(CII);                                     // Cars in intersection
        P(Block_sem[region1]);                      // Get into block
        message(REGION1, carnumber, cardirection, destination);
        V(entering_sem[cardirection]);              // Release entering sem          

        //Region 2
        P(Block_sem[region2]);                      // Get next block
        message(REGION2, carnumber, cardirection, destination);
        V(Block_sem[region1]);                      // Release privous block      


        //Region 3
        P(Block_sem[region3]);                      // Get next block
        message(REGION3, carnumber, cardirection, destination);
        V(Block_sem[region2]);                      // Release privous block


        //Leaving
        message(LEAVING, carnumber, cardirection, destination);

        V(Block_sem[region3]);                      // Release privous block
        V(CII);


}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnright(unsigned long cardirection,
          unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;

        //Destination direction
        int destination = (cardirection+3) % 4;

        //kprintf("car %d is turning right.\n", carnumber);

        //Block info
        int region1 = (cardirection + 4) % 4;


        //Get entering semaphore
        P(entering_sem[cardirection]);              // Enter

        //Approaching
        message(APPROACHING, carnumber, cardirection, destination);


        P(CII);                                     // Cars in intersection
        P(Block_sem[region1]);                      // Get into block
        //Region 1
        message(REGION1, carnumber, cardirection, destination);
        V(entering_sem[cardirection]);              // Release entering sem

        message(LEAVING, carnumber, cardirection, destination);

        V(Block_sem[region1]);                      // Release privous block
        V(CII);


}


/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long carnumber: holds car id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createcars().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */
 
static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
        int cardirection;
        int turn;

        #ifdef _DB_MODE_
        if (_DB_MODE_) {    
            kprintf("\nEnter stoplight thread.\n\n");
            kprintf("Current Thread: %s.\n\n", curthread->t_name);
            kprintf("Current Process: %s.\n\n", curproc->p_name);
        }
        #endif

        /*
         * Avoid unused variable and function warnings.
         */

        (void) unusedpointer;
        (void) carnumber;
    	(void) gostraight;
    	(void) turnleft;
    	(void) turnright;

        /*
         * cardirection is set randomly.
         */

        cardirection = random() % 4;

        /*
         * turn is set randomly.
         */

        turn = random() % 3;

        assert(turn <= 2);

        if (turn == 0) gostraight(cardirection, carnumber);
        else if (turn == 1) turnright(cardirection, carnumber);
        else turnleft(cardirection, carnumber);
}


/*
 * createcars()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createcars(int nargs,
           char ** args)
{
        int index, error;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

        /*
         * Start NCARS approachintersection() threads.
         */

        NW = sem_create("NW", 1); 
        NE = sem_create("NE", 1); 
        SE = sem_create("SE", 1); 
        SW = sem_create("SW", 1); 
        Block_sem[0] = NW;
        Block_sem[1] = NE;
        Block_sem[2] = SE;
        Block_sem[3] = SW;

        entering_NW = sem_create("entering_NW", 1); 
        entering_NE = sem_create("entering_NE", 1); 
        entering_SE = sem_create("entering_SE", 1); 
        entering_SW = sem_create("entering_SW", 1); 
        entering_sem[0] = entering_NW;
        entering_sem[1] = entering_NE;
        entering_sem[2] = entering_SE;
        entering_sem[3] = entering_SW;

        CII = sem_create("CII",3);

        // NS_straight = sem_create("NS_straight", 1);
        // EW_straight = sem_create("EW_straight", 1);
        // straight_priority[0] = NS_straight;
        // straight_priority[1] = EW_straight;

        for (index = 0; index < NCARS; index++) {

                error = thread_fork("approachintersection thread",
                                    NULL, // void * here
                                    index,
                                    approachintersection,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {
                        
                        panic("approachintersection: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

        return 0;
}
