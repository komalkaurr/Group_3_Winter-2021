#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <termios.h> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <math.h>  // for mathematical operation
#include "SocketClient.h"
#include <vector>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>


#define LOW 0
#define HIGH 1

using namespace std;
using namespace thingSpeak;

int fanPin=66;
int button=47;
int buzzeR=27;

ostringstream out_string;
string PATH="/sys/class/gpio/gpio";
fstream fd;

string thingSpeak_key="YHOLRJA0W7XREL23";
string smokeTField="field4";
string gasTField="field5";
string tempTField="field3";

// http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_addressing/lcd_addressing_index.html
// https://github.com/adafruit/Adafruit_Python_CharLCD/blob/master/Adafruit_CharLCD/Adafruit_CharLCD.py
// https://www.arduino.cc/en/Tutorial/LibraryExamples/HelloWorld


//Commands
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

//Entry flags
#define LCD_ENTRYRIGHT      0x00
#define LCD_ENTRYLEFT       0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

//Control flags
#define LCD_DISPLAYON   0x04
#define LCD_DISPLAYOFF  0x00
#define LCD_CURSORON    0x02
#define LCD_CURSOROFF   0x00
#define LCD_BLINKON     0x01
#define LCD_BLINKOFF    0x00

//Move flags
#define LCD_DISPLAYMOVE     0x08
#define LCD_CURSORMOVE      0x00
#define LCD_MOVERIGHT       0x04
#define LCD_MOVELEFT        0x00

//Function set flags
#define LCD_8BITMODE  0x10
#define LCD_4BITMODE  0x00
#define LCD_2LINE     0x08
#define LCD_1LINE     0x00
#define LCD_5x10DOTS  0x04
#define LCD_5x8DOTS   0x00

//Offset for up to 4 rows.
int LCD_ROW_OFFSETS[]= {0x00, 0x40, 0x14, 0x54};

//Char LCD plate GPIO numbers.
#define LCD_PLATE_RS     15
#define LCD_PLATE_RW     14
#define LCD_PLATE_EN     13
#define LCD_PLATE_D4     12
#define LCD_PLATE_D5     11
#define LCD_PLATE_D6     10
#define LCD_PLATE_D7     9
#define LCD_PLATE_RED    6
#define LCD_PLATE_GREEN  7
#define LCD_PLATE_BLUE   8

//Char LCD plate button names.
#define SELECT      0
#define RIGHT       1
#define DOWN        2
#define UP          3
#define LEFT        4


class LiquidCrystal{
    private:
        int _cols, _lines, _rs, _en, _d4, _d5, _d6, _d7, display_control, display_function, display_mode, _backlight;
        ostringstream out_string;
        string PATH = "/sys/class/gpio/gpio";
        fstream fd;

    public:
        void pinSetup(int pinNumber){
            out_string << pinNumber;
            string newS=PATH+out_string.str();
            out_string.str("");
            out_string.clear();
            fd.open(newS+"/direction");
            fd << "out";
            fd.close();
        }
        
        void outputs(int pinNumber, int val){
            out_string << pinNumber;
            string newS=PATH+out_string.str();
            out_string.str("");
            out_string.clear();
            out_string << val;
            fd.open(newS+"/value");
            fd << val;
            out_string.str("");
            out_string.clear();
            fd.close();    
        }

        void output(int pinNumber, bool logic){
            out_string << pinNumber;
            string newS=PATH+out_string.str();
            out_string.str("");
            out_string.clear();
            out_string << int(logic);
            fd.open(newS+"/value");
            fd << int(logic);
            out_string.str("");
            out_string.clear();
            fd.close();
        }


        void home(){
            /* Move the cursor back to its home (first line and first column).*/

            write8(LCD_RETURNHOME); // set cursor position to zero
            _delay_microseconds(3000);  // this command takes a long time!
        }

        void clear(){
            /* Clear the LCD.*/

            write8(LCD_CLEARDISPLAY);  // command to clear display
            _delay_microseconds(2000);  // 3000 microsecond sleep, clearing the display takes a long time
        }

        void _delay_microseconds(float tim){
            tim/=50000;
            out_string << tim;
            string cmd="sleep "+out_string.str();
            out_string.str("");
            out_string.clear();
            system(cmd.c_str());
        }

        void set_cursor(int col, int row){
            /* Move the cursor to an explicit column and row position. */

            // Clamp row to the last row of the display.

            if(row > _lines)
                row = _lines - 1;
            // Set location.
            
            write8(LCD_SETDDRAMADDR | (col + LCD_ROW_OFFSETS[row]));
        }

        void enable_display(int enable){
            /* Enable or disable the display.  Set enable to True to enable.*/

            if(enable)
                display_control |= LCD_DISPLAYON;
            else
                display_control &= ~LCD_DISPLAYON;
            write8(LCD_DISPLAYCONTROL | display_control);
        }

        void _pulse_enable(){
        // Pulse the clock enable line off, on, off to send command.
             output(_en, false);
             _delay_microseconds(1);      // 1 microsecond pause - enable pulse must be > 450ns
             output(_en, true);
             _delay_microseconds(1);      // 1 microsecond pause - enable pulse must be > 450ns
             output(_en, false);
             _delay_microseconds(1);
        }

        void show_cursor(bool show){
        /* Show or hide the cursor.  Cursor is shown if show is True.*/ 
            if(show)
                display_control |= LCD_CURSORON;
            else{
                display_control &= ~LCD_CURSORON;
                write8(LCD_DISPLAYCONTROL | display_control);
            }
        }

        void blink(bool blink){
            /* Turn on or off cursor blinking.  Set blink to True to enable blinking.*/
            if(blink)
                display_control |= LCD_BLINKON;
            else
                display_control &= ~LCD_BLINKON;
            write8(LCD_DISPLAYCONTROL | display_control);
        }

        void move_left(){
            /* Move display left one position.*/
            write8(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
        }

        void move_right(){
            /* Move display right one position.*/

            write8(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
        }

        void set_left_to_right(){
            /* Set text direction left to right.*/

            display_mode |= LCD_ENTRYLEFT;
            write8(LCD_ENTRYMODESET | display_mode);
        }

        void autoscroll(bool autoscroll){
            /*Autoscroll will 'right justify' text from the cursor if set True,
            otherwise it will 'left justify' the text.
            */
            if(autoscroll)
                display_mode |= LCD_ENTRYSHIFTINCREMENT;
            else
                display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
            write8(LCD_ENTRYMODESET | display_mode);
        }

        void message(string text){
            /* Write text to display.  Note that text can include newlines.*/
            int line = 0;
            // Iterate through each character.
            for(int i: text){
                // Advance to next line if character is a new line.
                if(i == 10){
                    line += 1;
                    // Move to left or right side depending on text direction.
                    int col=0; if(display_mode & LCD_ENTRYLEFT > 0){} else _cols-1;
                    set_cursor(col, line);
                }
                    // Write the character to the display.
                else
                    write8(i, true);
            }
        }

        void write8(int val, bool char_mode=false){
          /*
            Write 8-bit value in character or data mode.  Value should be an int
            value from 0-255, and char_mode is True if character data or False if
            non-character data (default).
          */

            _delay_microseconds(1000);
            output(_rs, char_mode);
            outputs(_d4,((val >> 4) & 1) > 0);
            outputs(_d5,((val >> 5) & 1) > 0);
            outputs(_d6,((val >> 6) & 1) > 0);
            outputs(_d7,((val >> 7) & 1) > 0);
            _pulse_enable();
            
            outputs(_d4,(val        & 1) > 0);
            outputs(_d5,((val >> 1) & 1) > 0);
            outputs(_d6,((val >> 2) & 1) > 0);
            outputs(_d7,((val >> 3) & 1) > 0);
            _pulse_enable();
            

        }

        LiquidCrystal(int rs, int en, int d4, int d5, int d6, int d7, int cols, int lines, int backlight=0){
            _cols=cols;
            _lines=lines;
            _rs=rs;
            _en=en;
            _d4=d4;
            _d5=d5;
            _d6=d6;
            _d7=d7;
            _backlight=backlight;
            pinSetup(rs);
            pinSetup(en);
            pinSetup(d4);
            pinSetup(d5);
            pinSetup(d6);
            pinSetup(d7);
            
            write8(0x33);
            write8(0x32);
            display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
            display_function = LCD_4BITMODE | LCD_1LINE | LCD_2LINE | LCD_5x8DOTS;
            display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

            // write registers
            write8(LCD_DISPLAYCONTROL | display_control);
            write8(LCD_FUNCTIONSET | display_function);
            write8(LCD_ENTRYMODESET | display_mode);  // set the entry mode
            clear();
        }
        ~LiquidCrystal(){

        }
};

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
            char recieve[300];
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

void pinSetup(int pinNumber, string direction){
    out_string << pinNumber;
    string newS=PATH+out_string.str();
    out_string.str("");
    out_string.clear();
    fd.open(newS+"/direction");
    fd << direction;
    fd.close();

}

 void output(int pinNumber, bool logic){
    out_string << pinNumber;
    string newS=PATH+out_string.str();
    out_string.str("");
    out_string.clear();
    out_string << int(logic);
    fd.open(newS+"/value");
    fd << int(logic);
    out_string.str("");
    out_string.clear();
    fd.close();
}

void speak(int amount, string field_name){
   ostringstream head, data;
   cout << "Starting ThingSpeak Example" << endl;
   SocketClient sc("api.thingspeak.com",80);
   data << field_name+"=" << amount << endl;
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

int mq2_sensor_val()  // generating a temerture function
{
   int gas_sensor;  // generating a local variable
   fstream f; // generating a stream for file
   f.open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw"); //  config P9_39 AIN of BBBW 
   f>>gas_sensor; // giving file address to local variable
   return (int)gas_sensor; //returning integer value to getTemp function
}

int mq9_sensor_val()  // generating a temerture function
{
   int Smoke_sensor;  // generating a local variable
   fstream f; // generating a stream for file
   f.open("/sys/bus/iio/devices/iio:device0/in_voltage1_raw"); //  config P9_40 AIN of BBBW 
   f>>Smoke_sensor; // giving file address to local variable
   return (int)Smoke_sensor; //returning integer value to getTemp function
}

int getTemp()  // generating a temerture function
{
   float calculated_temp;  // generating a local variable
   fstream f; // generating a stream for file
   f.open("/sys/bus/iio/devices/iio:device0/in_voltage2_raw"); //  config P9_37 AIN of BBBW 
   f>>calculated_temp; // giving file address to local variable
   calculated_temp= (calculated_temp/22.753)/2;  // formula for calculating temp in celsius
   return (int)calculated_temp; //returning integer value to getTemp function
}

int main(){
    system("/home/debian/reciever/reciever/enable_uart.sh");
    Uart nodemcu("/dev/ttyO1",B9600);
    Uart xbee("/dev/ttyO4",B9600);
    pinSetup(fanPin,"out");
    pinSetup(buzzeR,"out");
    vector<string> container;
    vector<string> xyz;
    vector<string> orderr;
    string tempData;
    int col_change=0;
    output(fanPin,LOW);

    ostringstream ss;
    string msg,rcvrString,newEmail;
    float dex1,gas_sensor_output,b1 ;
    float dex2,smoke_sensor_output,b2; 

    // BeagleBone Black configuration:
    int lcd_rs    = 67;
    int lcd_en    = 68;
    int lcd_d4    = 44;
    int lcd_d5    = 26;
    int lcd_d6    = 46;
    int lcd_d7    = 65;

    // Define LCD column and row size for 16x2 LCD.
    int lcd_columns = 20;
    int lcd_rows    = 4;

    LiquidCrystal lcd(lcd_rs,lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7,lcd_columns, lcd_rows);

    while(1)
    {   
	lcd.set_cursor(0,0);
	lcd.clear();
        rcvrString=xbee.readline();
	cout<<rcvrString<<endl;
        if(rcvrString.length()>0){
            orderr.clear();
            cout<<rcvrString<<endl;
            output(buzzeR,HIGH);
            system("sleep 1");
            output(buzzeR,LOW);
            boost::split(container, rcvrString, boost::is_any_of("="));
            container.erase(container.begin());
            for(string a:container){
                tempData="";
                boost::split(xyz, a, boost::is_any_of("|"));
                // price.pop_back();
                int count=0;
                for(string b:xyz){
                    if(count==2){
                        tempData+=b;
                        break;
                    }
                    if(count!=1){
                        tempData+=b+" x ";
                    }
                     ++count;
                }
            orderr.push_back(tempData);
        }
        }
        dex1= mq2_sensor_val();
        dex2= mq9_sensor_val(); 
        smoke_sensor_output = (dex1 * 5)/ 4095;
        smoke_sensor_output=3.027*exp(smoke_sensor_output*1.0698);
        cout<<"Smoke: "<<smoke_sensor_output<<endl;
	lcd.set_cursor(0,0);
        ss << (int)smoke_sensor_output;
        msg="Smoke: "+ss.str()+" ppm";
        lcd.message(msg);
        ss.str("");
        ss.clear();
        
        b1=dex2;
        b1=(b1/4);
        dex2=(dex2/4095)*5;
        gas_sensor_output=3.027*exp(1.0698*dex2);
        lcd.set_cursor(0,1);
        ss << (int)gas_sensor_output;
	cout<<"Gas: "<<gas_sensor_output<<endl;
        msg="Gas: "+ss.str()+" ppm";    
        lcd.message(msg);
        ss.str("");
        ss.clear();
        
        if((int)gas_sensor_output>=300){
            speak((int)gas_sensor_output,gasTField);
            ss << (int)gas_sensor_output;
            msg="Gas Level is "+ss.str()+" ppm";
            nodemcu.writeData(msg.c_str());
	 
	    newEmail="python3 /home/debian/reciever/reciever/sendmail.py Gas " +ss.str();
	   cout<<newEmail<<endl;
	    system(newEmail.c_str());
            ss.str("");
            ss.clear();
	    
        }
        
        if((int)smoke_sensor_output > 620){
            speak(smoke_sensor_output,smokeTField);
            ss << (int)smoke_sensor_output;
            msg="Smoke Level is "+ss.str()+" ppm";
            nodemcu.writeData(msg.c_str());
		 newEmail="python3 /home/debian/reciever/reciever/sendmail.py Smoke " +ss.str();
	    cout<<newEmail<<endl;
	     system(newEmail.c_str());

            ss.str("");

            ss.clear();
           
        }
        
        int valu=getTemp();
	lcd.set_cursor(0,2);
	ss << valu;
	msg="Temp : "+ss.str()+" C";
	ss.str("");
	ss.clear();
	cout<<"Temp : "<<valu<<endl;
	lcd.message(msg);
        if (valu>= 20)
        {
            speak(valu,tempTField);
            output(fanPin,HIGH);         //Sending high (1) logic to GPIO_67.
            lcd.set_cursor(0,3);
            msg="Fan: ON";    
            lcd.message(msg);
            cout<<"fan is ON"<<endl;    
        }  
        else {            //delay
            output(fanPin,LOW);           //Sending Low(0) logic to GPIO_67
            lcd.set_cursor(0,3);
            msg="Fan: OFF";    
            lcd.message(msg);
            cout<<"fan is OFF"<<endl;
        }
	system("sleep 1");
	lcd.clear();
        if(orderr.size()>0){
            for(string each: orderr){
		if(col_change>=4){
			col_change=0;
			system("sleep 1");
			lcd.clear();
		}
                lcd.set_cursor(0,col_change++);
		 lcd.message(each);
            }
        }
	lcd.clear();
        }  
    return 0;
}
