include<stdio.h>      //This header defines various functions for performing input output operations
#include<fcntl.h>       //Used to define arguments for open ,close functions
#include<unistd.h>       //This header file is for misscelaneous functions declaration
#include<termios.h>      // It contains the definitions used by terminal input output interfaces
#include<string.h>           //This is used for defining strings etc.

int main(){
   int file, count;
   file = open("/dev/ttyO1", O_RDWR);               //this function open the file associated with Uart1
   struct termios options;                          //declaration of a struct options

   tcgetattr(file, &options);                       //This function get parameters associated with terminal and store them in termios
   options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;
   tcflush(file, TCIFLUSH);                           //This function is used to clear input and output queue associated with terminal files
   tcsetattr(file, TCSANOW, &options);

    // if (count = write(file, data, strlen(data))<0){
   //    perror("Failed to write to the output\n");
   // }
   // printf("Data write Successfully !!");
   // usleep(100000);
   // }
   // return 0;

 unsigned  char receive[100];                                //recieve array declaration
while(1)
{
  count = read(file, (void*)receive, 100);                 //This variable count the number of bytes

   if (count==0) printf("There was no data available to read!\n");
   else if(count>0)
{ 

