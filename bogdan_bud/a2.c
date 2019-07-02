#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>

///////////////////
pthread_mutex_t lock;
pthread_cond_t cond;

int sem_id;
void P(int sem_id, int sem_no)
{
    struct sembuf op = {sem_no, -1, 0};

    semop(sem_id, &op, 1);
}


void V(int sem_id, int sem_no)
{
    struct sembuf op = {sem_no, +1, 0};

    semop(sem_id, &op, 1);
}


///////////////////

//in process 3
int* statePROC3[4];
pthread_t th3[4];
int entry=0, exity=0;
int condP33=0;
int condP32=0;

pthread_mutex_t lock;
pthread_cond_t cond;

pthread_mutex_t lock2;
pthread_cond_t cond2;

pthread_mutex_t lock3;
pthread_cond_t cond3;
void* thFunction3(void* arg) {
	
	
	int* val = (int*) arg;
	if((*val)+1==3)
		condP33=1;
	
	
	
	if (pthread_mutex_lock(&lock) != 0)
	    {
			perror("Cannot take the lock");
			exit(4);
		}
	while (condP33==0) {
	if (pthread_cond_wait(&cond, &lock) != 0) {
		perror("Cannot wait for condition");
		exit(6);
	}
    }
    
    if (pthread_mutex_unlock(&lock) != 0) {
    	perror("Cannot release the lock");
    	exit(5);
    }
    
    if((*val+1)==4)
		P(sem_id, 5);
		
	info(BEGIN,3,*val+1);
	
	if (pthread_mutex_lock(&lock) != 0) {
    	perror("Cannot take the lock");
    	exit(4);
    }
    
    if (pthread_cond_signal(&cond)  != 0) {
    	perror("Cannot signal the condition waiters");
    	exit(7);
    }


    if (pthread_mutex_unlock(&lock) != 0) {
    	perror("Cannot release the lock");
    	exit(5);
    }
    
    
    if((*val)+1==2)
		condP32=1;
  
  
  
    if (pthread_mutex_lock(&lock2) != 0)
	    {
			perror("Cannot take the lock");
			exit(4);
		}
	while (condP32==0) {
	if (pthread_cond_wait(&cond2, &lock2) != 0) {
		perror("Cannot wait for condition");
		exit(6);
	}
    }
    
    if (pthread_mutex_unlock(&lock2) != 0) {
    	perror("Cannot release the lock");
    	exit(5);
    }
    
	info(END,3,*val+1);
	
	if((*val+1)==4)
		V(sem_id,4);
	
	if (pthread_mutex_lock(&lock2) != 0) {
    	perror("Cannot take the lock");
    	exit(4);
    }
    
    if (pthread_cond_signal(&cond2)  != 0) {
    	perror("Cannot signal the condition waiters");
    	exit(7);
    }


    if (pthread_mutex_unlock(&lock2) != 0) {
    	perror("Cannot release the lock");
    	exit(5);
    }
	  
	return 0;
}

//in process 4
int* statePROC4[37];
pthread_t th4[37];
int th_no=0;
void* thFunction4(void* arg) {
	
	int* val = (int*) arg;

	if((*val+1)!=12)
		{
			P(sem_id,0);
		}
	else
		{
			for(int i = 0; i < 3; i++) {
				P(sem_id, 2);
        }
		
		}
		
	info(BEGIN,4,*val+1);
	if((*val+1) != 12) {
        
        V(sem_id, 2);

        P(sem_id, 1);
    }

	info(END,4,*val+1);
	
	if((*val)+1 == 12) {
       
        semctl(sem_id, 1, SETVAL, 37);
    }
	
	V(sem_id, 0);
	return 0;
}

//in process 6
int* statePROC6[4];
pthread_t th6[4];

void* thFunction6(void* arg) {
	
	int* val = (int*) arg;
	
	
	
	P(sem_id,3);
	
	if((*val+1)==3)
		P(sem_id, 4);
	
	info(BEGIN,6,*val+1);
	info(END,6,*val+1);
	
	if((*val+1)==1)
		V(sem_id, 5);
	V(sem_id,3);
	
	return 0;
}


int main(){
    init();
	
	 sem_id = semget(IPC_PRIVATE, 6, IPC_CREAT | 0600);
    if (sem_id < 0) {
        perror("Error creating the semaphore set");
        exit(2);
    }
	//semaphore for proc 4
	semctl(sem_id, 0, SETVAL, 3);
	semctl(sem_id, 1, SETVAL, 0);
	semctl(sem_id, 2, SETVAL, 0);
	semctl(sem_id, 3, SETVAL, 4);
	semctl(sem_id, 4, SETVAL, 0);
	semctl(sem_id, 5, SETVAL, 0);
	//lock for proc 3
	 if (pthread_mutex_init(&lock, NULL) != 0) {
    	perror("Cannot initialize the lock");
    	exit(2);
    }
	
    if (pthread_cond_init(&cond, NULL) != 0) {
    	perror("Cannot initialize the condition variable");
    	exit(3);
    }
    
    if (pthread_mutex_init(&lock2, NULL) != 0) {
    	perror("Cannot initialize the lock");
    	exit(2);
    }
	
    if (pthread_cond_init(&cond2, NULL) != 0) {
    	perror("Cannot initialize the condition variable");
    	exit(3);
    }
	
	info(BEGIN, 1, 0);
	
	pid_t pid2 = fork();
  
switch (pid2) 
{
  case -1:
    {
		// error case
		perror("Cannot create a new child");
		exit(1);
	}
  case 0:
	{
		info(BEGIN, 2, 0);
		pid_t pid5=fork();
		
		switch (pid5) 
		{
			case -1:
				{
					// error case
					perror("Cannot create a new child");
					exit(1);
				}
			case 0:
				{
					
					info(BEGIN, 5, 0);
					pid_t pid6=fork();
					switch (pid6) 
					{
						case -1:
							{
								// error case
								perror("Cannot create a new child");
								exit(1);
							}
						case 0:
							{
								info(BEGIN, 6, 0);
								
								pid_t pid7=fork();
								switch (pid7) 
								{
									case -1:
										{
											// error case
											perror("Cannot create a new child");
											exit(1);
										}
									case 0:
										{
											info(BEGIN, 7, 0);
											info(END, 7, 0);
											break;
										}
									default:
										{
											waitpid(pid7,NULL,0);
											
											int v[4];
											for (int i=0; i<4; i++)
												{
													v[i]=i;
													pthread_create(&th6[i], NULL, thFunction6, &v[i]);
												}
							
											for (int i=0; i<4; i++)
												{
													pthread_join(th6[i], (void**) &statePROC6[i]);
												}
								
								
											info(END,6,0);
										}
								}
								
								break;
							}
						default:
							{
								waitpid(pid6,NULL,0);
								info(END, 5, 0);
							}
						
					}
					
					
					break;
				}
			default:
				{
					waitpid(pid5,NULL,0);
					info(END, 2, 0);
				}
		}
		
		
		
		
		break;
    
    }
  default:
    {
		pid_t pid3 = fork();
		
		switch (pid3) 
		{
			case -1:
				{
					// error case
					perror("Cannot create a new child");
					exit(1);
				}
			case 0:
				{
					info(BEGIN, 3, 0);
					
					pid_t pid4=fork();
					
					switch (pid4) 
					{
					case -1:
						{
							// error case
							perror("Cannot create a new child");
							exit(1);
						}
					case 0:
						{
							info(BEGIN, 4, 0);
							
							int v[37];
							
							for (int i=0; i<37; i++)
								{
									v[i]=i;
									pthread_create(&th4[i], NULL, thFunction4, &v[i]);
								}
							
							for (int i=0; i<37; i++)
								{
									pthread_join(th4[i], (void**) &statePROC4[i]);
								}
							
							info(END, 4, 0);
							break;
						}
					default:
						{
							
							
							int v[4];
							for (int i=0; i<4; i++)
								{
									v[i]=i;
									pthread_create(&th3[i], NULL, thFunction3, &v[i]);
								}
							
							for (int i=0; i<4; i++)
								{
									pthread_join(th3[i], (void**) &statePROC3[i]);
								}
							waitpid(pid4,NULL,0);
							info(END, 3, 0);
						}
					
					
					}
					break;
				}
			default:
				{
					waitpid(pid2,NULL,0);
					waitpid(pid3,NULL,0);
					info(END, 1, 0);
				}
	

		}
    
	}
} 

    
    
    
    return 0;
}
