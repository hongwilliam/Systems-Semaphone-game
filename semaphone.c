#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>

#define KEY 831

int main(int argc, char *argv[]){

  int sem;
  int amt = 1;

  char* cmd = argv[1];
  //printf("%s\n",cmd);

  int shmd;
  int f;

  if(!strcmp(cmd,"-c")){
    if(argv[2]){
      int val = atoi(argv[2]);
      shmd = shmget(KEY, sizeof(char*) * val, IPC_CREAT | IPC_EXCL | 0600);
      sem = semget(KEY,amt,IPC_CREAT | IPC_EXCL | 0600);
      f = open("story.txt", O_CREAT | O_TRUNC | O_EXCL, 0600);
      if(sem == -1)
	       printf("semaphore already exists\n");
      else{
      	int v = semctl(sem,0,SETVAL,val);
      	printf("semaphore created: %d\n",sem);
      	printf("value set: %d\n",v);
      }
    }
    else
      printf("Need a value\n");
  }

  else if(!strcmp(cmd,"-r")){
    sem = semget(KEY,amt,0600);
    int v = semctl(sem,0,IPC_RMID);
    shmd = shmget(KEY, sizeof(char*), 0600);
    shmdt(&shmd);

    if(v == -1)
      printf("%s\n",strerror(errno));
    else{
      printf("semaphore removed: %d \n",v);
      f = open("story.txt", O_RDONLY | O_TRUNC, 0600);
      int *buff = malloc(sizeof(char*));
      read(f, buff, sizeof(buff));
    }
  }


  else if(!strcmp(cmd,"-v")){
    sem = semget(KEY,amt,0600);
    int v = semctl(sem,0,GETVAL);

    if(v == -1)
      printf("%s\n",strerror(errno));
    else
      printf("semaphore value: %d\n",v);
  }



  else
    printf("Not a valid command\n");

}
