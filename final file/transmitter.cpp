#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <map>
#include <unistd.h>
#include <termios.h>
#include <iostream>
//#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "SocketClient.h"
#include <vector>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace thingSpeak;

string thingSpeak_key="49ZOKETVBQLQUUWK";

class Uart{
    private:
        string portname;
        size_t baud_rate;
        int    file_descriptor;
        int    byte_counter;

    public:
        Uart(string portname, size_t baud_rate){
            this->portname=portname;
            this->baud_rate=baud_rate;
            file_descriptor=open(this->portname.c_str(), O_RDWR);
            
            if(file_descriptor<0){
                cout<<"Port is Busy or Can't open for communication !!";
                exit(0);
            } else {
                cout<<"Connection Succeed!!"<<endl;
            }

            struct termios options;

            tcgetattr(file_descriptor, &options);
            options.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
            options.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
            options.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
            options.c_cflag |= CS8; // 8 bits per byte (most common)
            options.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
            options.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

            options.c_lflag &= ~ICANON;
            options.c_lflag &= ~ECHO; // Disable echo
            options.c_lflag &= ~ECHOE; // Disable erasure
            options.c_lflag &= ~ECHONL; // Disable new-line echo
            options.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
            options.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
            options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

            options.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
            options.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
            // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
            // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

            options.c_cc[VTIME] = 5;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
            options.c_cc[VMIN] = 0;

            // Set in/out baud rate to be 9600
            cfsetispeed(&options, this->baud_rate);
            cfsetospeed(&options, this->baud_rate);
            // options.c_cflag = this->baud_rate | CS8 | CREAD | CLOCAL;
            // options.c_cflag &= ~CRTSCTS;
            // options.c_iflag = IGNPAR | ICRNL;
            // options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
            // options.c_oflag &= ~OPOST;
            // options.c_oflag &= ~ONLCR;

            tcsetattr(file_descriptor, TCSANOW, &options);

        }
        ~Uart(){
            close(this->file_descriptor);
            cout<<"Connection Closed !!"<<endl;
        }

        string readline(){
            char recieve[80];
            char* data=(char *)"\0";
            memset(&recieve, '\0', sizeof(recieve));
            int array_size=sizeof(recieve) / sizeof(char);
            this->byte_counter=read(file_descriptor, &recieve, sizeof(recieve));
            if(this->byte_counter>0){
                data = (char *) malloc(this->byte_counter*sizeof(char));
                data=recieve;
                return data;
            }
            string temp(data);
            return temp;
      }
      void writeData(string data){
             write(file_descriptor, data.c_str(), data.length());
      }
};

map<string,string> foodDict={
    {"PZA","Pizza"},
    {"BGR","Burger"},
    {"ICM","Ice Cream"},
    {"COF","Coffee"}
};

vector<string> container;
vector<string> price;
int totalPrice;

//void finish_with_error(MYSQL *con)
//{
  //fprintf(stderr, "%s\n", mysql_error(con));
  //mysql_close(con);
 // exit(1);
//}

string mapping(string input){
    for(auto iter: foodDict){
        // cout<<iter.second;
        string key=iter.first;
        string value=iter.second;
        int index=input.find(key);
        if(index <= input.length()){
            input.replace(index,input.length(),value);
        }
    }
    return input;
    
}

void speak(int amount, int qty){
   ostringstream head, data;
   cout << "Starting ThingSpeak Example" << endl;
   SocketClient sc("api.thingspeak.com",80);
   data << "field1="<<amount<<"&field2=" << qty << endl;
   sc.connectToServer();
   head << "POST /update HTTP/1.1\n"
        << "Host: api.thingspeak.com\n"
        << "Connection: close\n"
        << "X-THINGSPEAKAPIKEY: " + thingSpeak_key +"\n"
        << "Content-Type: application/x-www-form-urlencoded\n"
        << "Content-Length:" << string(data.str()).length() << "\n\n";
   sc.send(string(head.str()));
   sc.send(string(data.str()));
   string rec = sc.receive(1024);
//    cout << "[" << rec << "]" << endl;
   cout << "End of ThingSpeak Example" << endl;
}


int main(int argc, char **argv){
    ////MYSQL *con = mysql_init(NULL);
    //if (con == NULL)
    //{
      //  fprintf(stderr, "%s\n", mysql_error(con));
       // exit(1);
  //  }

    //if (mysql_real_connect(con, "localhost", "root", "Test@1234",
     //       "orders", 0, NULL, 0) == NULL){
      //  finish_with_error(con);
    //}
    string nextionString,dataContainer,tempp;
    int quantity=0,prices=0,totalQty=0;
    vector<string> tempCnt;
    Uart nextion("/dev/ttyO4",B9600);
    Uart xbee("/dev/ttyO1",B9600);
    
    while(true){
        nextionString=nextion.readline();
        tempCnt.clear();
        // cin>>nextionString;
        
        if(nextionString=="confirm") {
            boost::split(container, dataContainer, boost::is_any_of("="));
            container.erase(container.begin());
            string tempData="";
            for(string a:container){
                boost::split(price, a, boost::is_any_of("|"));
                // price.pop_back();
                // int count=0;
                // for(string b:price){
                //     if(count==1){
                //         stringstream conv(b);
                //         conv >> prices;
                //         conv.str("");
                //         totalPrice+=quantity*prices;
                //         break;
                //     }
                //     stringstream conv(b);
                //     conv >> quantity;
                //     conv.str("");
                //     ++count;
                // }
                string tempp=mapping(a);
                vector<string> tempCnt;
                boost::split(tempCnt, tempp, boost::is_any_of("|"));
	        stringstream co(tempCnt[0]);
                int dum;
                co >> dum;
                co.str("");
                co.clear();
                totalQty+=dum;

                string qry="insert into food_order(item,qty,price) values('"+tempCnt[2]+"',"+tempCnt[0]+","+tempCnt[1]+")";            
                stringstream conv(tempCnt[0]);
                conv >> quantity;
    		conv.str("");
		conv.clear();
		//if (mysql_query(con, qry.c_str())){
                   // finish_with_error(con);
                //}//
                tempData+="="+tempp;
                
            }
            cout<<tempData<<endl;
            if(totalPrice>0){
		cout<<"Amit"<<endl;
		speak(totalPrice,totalQty);
                xbee.writeData(tempData);
		totalQty=0;
            }
            cout<<"Amount : "<<totalPrice<<endl;
            cout<<"Reciever : "<<tempData<<endl;
        }
        else if(nextionString.length()!=0){
            dataContainer=nextionString;
	    cout<<dataContainer<<endl;
            boost::split(container, dataContainer, boost::is_any_of("="));
            container.erase(container.begin());
            string tempData="";
            totalPrice=0;
            for(string a:container){
                boost::split(price, a, boost::is_any_of("|"));
                price.pop_back();
                int count=0;

                for(string b:price){
                    if(count==1){
                        stringstream conv(b);
                        conv >> prices;
                        conv.str("");
                        totalPrice+=quantity*prices;
                        break;
                    }
                    stringstream conv(b);
                    conv >> quantity;
                    conv.str("");
                    ++count;
                }
                // string tempp=mapping(a);
                // vector<string> tempCnt;
                // boost::split(tempCnt, tempp, boost::is_any_of("|"));
                // string qry="insert into food_order(item,qty,price) values('"+tempCnt[2]+"',"+tempCnt[0]+","+tempCnt[1]+")";            
                // if (mysql_query(con, qry.c_str())){
                //     finish_with_error(con);
                // }
                // tempData+="="+tempp;
            }
            cout<<tempData<<endl;
            if(totalPrice>0){
                // speak(totalPrice);
                stringstream ss;
                ss << totalPrice;
                string sendNextion="python3 /home/debian/demoFolder/price_calculator.py "+ss.str();
                ss.str("");
                ss.clear();
                system(sendNextion.c_str());
            }
            }

        }
        // sleep(1);
        // fflush(stdout);
   // mysql_close(con);

    return 0;
}


// `mysql_config --cflags --libs`
