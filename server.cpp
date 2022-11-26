#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define FileName "data.data"
#define DataString "Now is the winter of our discontent\nMade glorious summer by this sun of York\n"

void report_and_exit (const char* msg) {
  perror(msg);
  exit(-1); // Faiure
}

int main() {

  struct flock lock;

  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0; 
  lock.l_pid = getpid();

  int file_descriptor;

  if ((file_descriptor = open(FileName, O_RDWR | O_CREAT, 0666)) < 0) {
    report_and_exit("open failed!");
  }

  if (fcntl(file_descriptor, F_SETLK, &lock) < 0) {
    report_and_exit("fcntl failed to get the lock....");
  }
  else {
    write(file_descriptor, DataString, strlen(DataString));
    fprintf(stderr, "Process %d has written to data file...\n", lock.l_pid);
  }

  sleep(30); 
  // Release the lock
  lock.l_type = F_UNLCK;

  if (fcntl(file_descriptor, F_SETLK, &lock) < 0) {
    report_and_exit("Explicit unlocking failed!");
  } 
  else {
    close(file_descriptor);
  }

  return EXIT_SUCCESS;

  return 0;
}
