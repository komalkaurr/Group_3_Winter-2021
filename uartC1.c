#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

int main(){
   int file, count;
   file = open("/dev/ttyO1", O_RDWR);
  printf("Output of file : %d",file);
   struct termios options;

   tcgetattr(file, &options);
   options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;
   tcflush(file, TCIFLUSH);
   tcsetattr(file, TCSANOW, &options);

   char * data="hello\0";
   while(1){
   // if (count = write(file, data, strlen(data))<0){
   //    perror("Failed to write to the output\n");
   // }
   // printf("Data write Successfully !!");
   // usleep(100000);
   // }
   // return 0;

   char *receive;
   if ((count = read(file, (void*)receive, 100))<0){ 
	perror("Failed to read from the input\n"); return -1;
   }
   if (count==0) printf("There was no data available to read!\n"); 
   else { 
     receive[count]=0;
      printf("The following was read in [%d]: %s\n",count,receive);
   }
sleep(2);
}
}
