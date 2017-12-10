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

int main(int argc, char *argv[]){

  int semid;
  union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array; 
    struct seminfo *__buf;
  };
  int shmid;
  int f;
  int amt = 1;

  char* cmd = argv[1];
  //printf("%s\n",cmd);


  //Creating
  if(!strcmp(cmd,"-c")){
    //create semaphore
    semid = semget(KEY,amt,IPC_CREAT | IPC_EXCL | 0600);
    if(semid == -1)
      printf("could not create semaphore: %s\n",strerror(errno));
    else{
      union semun su;
      su.val = 10;
      int v = semctl(semid,0,SETVAL,su);
      printf("semaphore created: %d\n",semid);
      printf("value set: %d\n",v);
    }
    
    //create shared memory
    shmid = shmget(KEY, sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
    if(shmid == -1)
      printf("could not create shared memory: %s\n",strerror(errno));
    else
      printf("shared memory created: %d\n",shmid);

    //create story file
    f = open("story.txt", O_CREAT | O_EXCL | O_TRUNC, 0600);
    if(f == -1)
      printf("could not create story file: %s\n",strerror(errno));
    else{
      printf("story file created: %d\n",f);
      close(f);
    }
    
  }

  
  //Removing
  else if(!strcmp(cmd,"-r")){
    semid = semget(KEY,amt,0600);
    int v = semctl(semid,0,GETVAL);
    
    //block if semaphore val is 0
    while(v == 0)
      v = semctl(semid,0,GETVAL);
    
    //remove semaphore
    v = semctl(semid,0,IPC_RMID);
    if(v == -1)
      printf("could not remove semaphore: %s\n",strerror(errno));
    else
      printf("semaphore removed: %d \n",v);
    
    //remove shared memory
    shmid = shmget(KEY, sizeof(int), 0600);
    char* shm = shmat(shmid,0,0);
    int s = shmdt(shm);    
    if(s == -1)
      printf("could not detach shared memory: %s\n",strerror(errno));
    else
      printf("memory detached: %d\n", s);
    int s2 = shmctl(shmid,IPC_RMID,0);
    if(s2 == -1)
      printf("could not remove shared memory: %s\n",strerror(errno));
    else{
      printf("memory removed: %d\n", s2);
    }

    //remove story
    f = open("story.txt", O_RDONLY, 0600);
    if(f == -1)
      printf("could not access story: %s\n",strerror(errno));
    else{
      //get the length of the story
      struct stat st; 
      stat("story.txt", &st);
      size_t size = st.st_size;
      char *buffer = calloc(size,sizeof(char)); //calloc the necessary length
      read(f, buffer, size);
      printf("\nstory: \n%s\n",buffer);
      free(buffer);
      close(f);
      remove("story.txt");
    }
  }

  
  //Viewing
  else if(!strcmp(cmd,"-v")){
    f = open("story.txt", O_RDONLY, 0600);   

    if(f == -1)
      printf("could not access story: %s\n",strerror(errno));
    else{
      //get the length of the story
      struct stat st;
      stat("story.txt", &st);
      size_t size = st.st_size;
      char *buffer = calloc(size,sizeof(char)); //calloc the necessary length
      read(f, buffer, size);
      printf("story: \n%s\n",buffer);
      close(f);
      free(buffer);
    }
  }


  else
    printf("Not a valid command\n");

}
