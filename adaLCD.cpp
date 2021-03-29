#include<iostream>
#include<sstream>
#include<fstream>
#include<cstring>
#include<unistd.h>
using namespace std;
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

int main(){
    // BeagleBone Black configuration:
    int lcd_rs    = 67;
    int lcd_en    = 68;
    int lcd_d4    = 44;
    int lcd_d5    = 26;
    int lcd_d6    = 46;
    int lcd_d7    = 65;

    // Define LCD column and row size for 16x2 LCD.
    int lcd_columns = 20;
    int lcd_rows    =  4;

    LiquidCrystal lcd(lcd_rs,lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7,lcd_columns, lcd_rows);
for (int i = 0; i < 5; i++)
{
    lcd.set_cursor(1,0);
    lcd.message("Item       Quanity  ");

    lcd.set_cursor(1,1);
    lcd.message("Pizza        1");

    lcd.set_cursor(1,2);
    lcd.message("Icecream     4");
    lcd.set_cursor(1,3);
    lcd.message("Burger       2");
lcd.set_cursor(1,4);
    lcd.message("Juice        1    ");
    // Wait 5 seconds
    sleep(1);
}
    return 0;
}
