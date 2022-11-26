/*
  FeatureAdd: HamzaMateen
  credits: https://opensource.com/article/19/4/interprocess-communication-linux-storage
  
 * Robust version of POSIX IPC via file sharing.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FileName "data.data"

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

int main() {
  struct flock lock;

  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  int file_descriptor;

  if ((file_descriptor = open(FileName, O_RDONLY)) < 0) {
    report_and_exit("open to read failed!");
  }

  acquire_readlock:
  fcntl(file_descriptor, F_GETLK, &lock); 
  // checks for the specified lock(in this case l_type == F_WRLCK) and changes l_type to F_UNLCK if the lock does not exist. (Safety mechanism in case the server/producer didn't set the l_type to F_UNLCK upon release of the F_WRLCK)
  
  if (lock.l_type != F_UNLCK) {
    puts("file is still write locked, trying again in 5 seconds .... ");
    sleep(5);
    
    goto acquire_readlock;
  }  

  lock.l_type = F_RDLCK;
  if (fcntl(file_descriptor, F_SETLK, &lock) < 0) {
    report_and_exit("can't get a read-only lock!");
  }

  // read the bytes now
  int character;
  while(read(file_descriptor, &character, 1) > 0) {
    write(STDOUT_FILENO, &character, 1);
  } 

  lock.l_type = F_UNLCK;

  if (fcntl(file_descriptor, F_SETLK, &lock) < 0) {
    report_and_exit("explicit unlocking failed...");
  }

  close (file_descriptor);

  return EXIT_SUCCESS;
}