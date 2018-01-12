/*
 * catlock.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use LOCKS/CV'S to solve the cat syncronization problem in 
 * this file.
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
#include <machine/spl.h>


/*
 * 
 * Constants
 *
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2

// Locks and cvs

struct lock *entering, *bowls[NFOODBOWLS], *counting;
struct cv *kitchen_wait;

int catEating, mouseEating;


/*
 * 
 * Function Definitions
 * 
 */

/* who should be "cat" or "mouse" */
static void
lock_eat(const char *who, int num, int bowl, int iteration)
{
        kprintf("%s: %d starts eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
        clocksleep(1);
        kprintf("%s: %d ends eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
}

/*
 * catlock()
 *
 * Arguments:
 *      void * unusedarg: Currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS -
 *      1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
catlock(void * unusedarg, 
        unsigned long catnumber)
{
        assert(catnumber <= NCATS);
        int i;
        int times = 0;

        while(times < 4){
            int availableBowl = -1;

            lock_acquire(entering);

            assert(lock_do_i_hold(entering));

            while(1){
                for(i=0;i<NFOODBOWLS;i++){
                    if(bowls[i]->in_use == 0)
                        {availableBowl = i;break;}
                }
                if(availableBowl>=0) break;
                else cv_wait(kitchen_wait,entering);
                // if(availableBowl<0) {
                //     //kprintf("cat: No space!\n");
                //     cv_wait(cat_wait, entering);
                // }
            }

            lock_acquire(bowls[availableBowl]);
            lock_acquire(counting);
            if(mouseEating > 0){
                //kprintf("cat: mouse is eating!\n");
                lock_release(bowls[availableBowl]);
                lock_release(counting);
                cv_wait(kitchen_wait, entering);
                lock_release(entering);
                continue;
            }
            catEating++;
            lock_release(counting);
            //kprintf("cat:I start eating!\n");
            lock_release(entering);
            // everybody fight for your food!
            cv_broadcast(kitchen_wait, entering);

            

            lock_eat("cat", catnumber, availableBowl+1, times);
            times++;
            lock_acquire(counting);
            catEating--;
            lock_release(counting);

            // I'm LEAVING
            lock_release(bowls[availableBowl]);

            // everybody fight for your food!
            cv_broadcast(kitchen_wait, entering);
        }

        /*
         * Avoid unused variable warnings.
         */

        (void) unusedarg;
        (void) catnumber;
}

    

/*
 * mouselock()
 *
 * Arguments:
 *      void * unusedarg: Currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
mouselock(void * unusedarg,
          unsigned long mousenumber)
{
        assert(mousenumber <= NMICE);
        int i;
        int times = 0;
        while(times < 4){
            int availableBowl = -1;

            lock_acquire(entering);

            assert(lock_do_i_hold(entering));

            while(1){
                for(i=0;i<NFOODBOWLS;i++){
                    if(bowls[i]->in_use == 1)
                        {availableBowl = i;break;}
                }
                if(availableBowl>=0) break;
                else cv_wait(kitchen_wait, entering);
                // if(availableBowl<0) {
                //     //kprintf("mouse: No space\?\?!!\n");
                //     cv_wait(mouse_wait, entering);
                // }
            }

            lock_acquire(bowls[availableBowl]);
            lock_acquire(counting);
            if(catEating > 0){
                //kprintf("mouse: cat is eating!\n");
                lock_release(bowls[availableBowl]);
                lock_release(counting);
                cv_wait(kitchen_wait, entering);
                lock_release(entering);
                continue;
            }
            mouseEating++;
            lock_release(counting);
            //kprintf("mouse:I start eating!\n");
            // everybody fight for your food!
            lock_release(entering);
            cv_broadcast(kitchen_wait, entering);
            

            lock_eat("mouse", mousenumber, availableBowl+1, times);
            times++;
            lock_acquire(counting);
            mouseEating--;
            lock_release(counting);

            // I'm LEAVING
            lock_release(bowls[availableBowl]);

            // everybody fight for your food!
            cv_broadcast(kitchen_wait, entering);
        }

        /*
         * Avoid unused variable warnings.
         */
        
        (void) unusedarg;
        (void) mousenumber;
}


/*
 * catmouselock()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catlock() and mouselock() threads.  Change
 *      this code as necessary for your solution.
 */

int
catmouselock(int nargs,
             char ** args)
{
        int index, error;
   
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

        entering = lock_create("entering");
        counting = lock_create("counting");
        for(index = 0; index < NFOODBOWLS; index++){
            bowls[index] = lock_create("bowl");
        }
        
        
        kitchen_wait = cv_create("kitchen_wait");

        catEating = 0;
        mouseEating = 0;

   
        /*
         * Start NCATS catlock() threads.
         */


        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catlock thread", 
                                    NULL, 
                                    index, 
                                    catlock, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catlock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        /*
         * Start NMICE mouselock() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mouselock thread", 
                                    NULL, 
                                    index, 
                                    mouselock, 
                                    NULL
                                    );
      
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mouselock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        // // free memory
        // lock_destroy(entering);
        // lock_destroy(cat_available);
        // lock_destroy(mouse_available);
        // cv_destroy(cat_wait);
        // cv_destroy(mouse_wait);
        // for (index = 0; index < NFOODBOWLS; index++)
        //     cv_destroy(bowls[index]);

        return 0;
}

/*
 * check_bowls()
 *
 * Arguments:
 *      int *availableSlot: arg passed by reference. Return last available bowl.
 *
 * Returns:
 *      1 if there's bowl available.
 *
 * Notes:
 *      Functions used to check available bowl.
 */


// int check_bowls(int *availableSlot){
//     for (i=0; i<NFOODBOWLS; i++){
//         if(bowls[i] -> in_use == 0) {
//             *availableSlot = i;
//             return 1;
//         }
//     }
//     return 0;
// }

/*
 * End of catlock.c
 */

