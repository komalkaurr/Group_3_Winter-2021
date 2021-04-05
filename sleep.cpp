#include <iostream>
#include <unistd.h>

using namespace std;
int main() 
{
cout << "system going for sleep for 20 sec"<< endl; 
  system("sudo rtcwake -d /dev/rtc0 -m standby -s 20");   // putting system to sleep for 20 sec  
   
 
   return 0;
}