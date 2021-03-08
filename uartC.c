#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

int main(){
   int file, count;
   file = open("/dev/ttyO1", O_RDWR);

   struct termios options;

   tcgetattr(file, &options);
   options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;
   tcflush(file, TCIFLUSH);
   tcsetattr(file, TCSANOW, &options);

   char * data="hello\0";
   if ((count = write(file, data, strlen(data)<0){
      perror("Failed to write to the output\n");
      return -1;
   }
   usleep(100000);

   return 0;
}
