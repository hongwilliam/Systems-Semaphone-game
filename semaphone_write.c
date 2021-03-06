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
#include <fcntl.h>
#include <sys/stat.h>
#define KEY 832

//The main writing program that takes the input and adds it to the story file
int main(){
  int semid = semget(KEY, 1, 0600);

  //if semid fails
  if(semid == -1){
    printf("semaphore error: %s\n",strerror(errno));
    return -1;
  }
  
  //this is for the operation argument for semop
  struct sembuf atomic_op;
  atomic_op.sem_op = -1; //for down
  atomic_op.sem_num = 0; //index of semaphore we want to control
  atomic_op.sem_flg = SEM_UNDO; //allow OS to undo given operation
  semop(semid, &atomic_op, 1); //now performing operation on the semaphore

  //access shared memory to contain last line size
  int shmid = shmget(KEY, sizeof(int), 0600);
  if(shmid == -1){
    printf("shared memory error: %s\n",strerror(errno));
    return -1;
  }
  
  //attach shared memory segment by setting pointer to segment
  int *length = shmat(shmid, 0, 0);

  //open the story file
  int fd = open("story.txt", O_RDWR | O_APPEND, 0600);
  if(fd == -1){
    printf("file error: %s\n",strerror(errno));
    return -1;
  }
  
  //we want to change current position in the opened file
  //move position by the offset relative to END of the file
  lseek(fd, (*length) * -1, SEEK_END);
  char *buffer = malloc((int)*length); //malloc the necessary length
  read(fd, buffer, *length);
  printf("Last line added to story: %s \n", buffer); //display last line added!

  //prompt use for next line
  printf("Enter next line: \n");
  char input[1024];
  fgets(input, 1024, stdin);

  //detach the variable length from the shared memory segment
  //this to to reuse for the next inputted line
  *length = strlen(input);
  shmdt(length);

  //write the new input to the file
  write(fd, input, strlen(input) );
  close(fd);

  //up or "release" the semaphore
  atomic_op.sem_op = 1;
  semop(semid, &atomic_op, 1);

}
