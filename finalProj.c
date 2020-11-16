#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>     /* key_t, sem_t, pid_t      */
#include <sys/wait.h>       /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>  
#include <unistd.h>

#define NUM_OF_TRAINERS 5
#define NUM_OF_COUCHES 6
       

int main (int argc, char **argv){
    int i;                        /*loop variables*/   
    sem_t *sem;                   /*      synch semaphore         *//*shared */
    pid_t pid;                    /*      fork pid                */
    int *p;                       /*      shared variable         *//*shared */

    p = (int*)malloc(sizeof(int));
	*p = 0;

    /* initialize semaphores for shared processes */
    sem = sem_open ("FitnessCentre", O_CREAT | O_EXCL, 0644, NUM_OF_TRAINERS); 
   
    printf ("semaphores initialized.\n\n");


    /* fork child processes */
    for (i = 0; i < 7; i++){         // I assumed 7 as no.of customer. From Payton's code "Customer" to be integrate.
        pid = fork ();
        if (pid < 0) {
        /* check for error      */
            sem_unlink ("FitnessCentre");   
            sem_close(sem);  
            /* unlink prevents the semaphore existing forever */
            /* if a crash occurs during the execution         */
            printf ("Fork error.\n");
        }
        else if (pid == 0)
            break;                  /* child processes */
    }


    if (pid != 0){
        /* wait for all children to exit */
        while (pid = waitpid (-1, NULL, 0)){
            if (errno == ECHILD)
                break;
        }

        printf ("\nParent: All children have exited.\n");


        /* cleanup semaphores */
        sem_unlink ("FitnessCentre");   
        sem_close(sem);  
        /* unlink prevents the semaphore existing forever */
        /* if a crash occurs during the execution         */
        exit (0);
    }

    else{
        sem_wait (sem);           /* P operation */
        printf ("  Customer(%d) is in critical section.\n", i);
        sleep (1);
        *p += i % NUM_OF_TRAINERS;              /* increment *p by 0, 1 or 2 based on i */
        printf ("  Customer(%d) completed training with Trainer %d.\n", i, *p);
        sem_post (sem);           /* V operation */
        exit (0);
    }
}
