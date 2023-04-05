#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <Keypad.h>

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

//the definiens of 16bit mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV mylcd(ILI9486,40,38,39,-1,41); //model,cs,cd,wr,rd,reset

//define some colour values
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'D', '#', '0', '*'},
  {'C', '9', '8', '7'},
  {'B', '6', '5', '4'},
  {'A', '3', '2', '1'}
};
 
// Connections to Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
 
// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
 

String temp = "Temperature ";
String c = "C";
String ext = "Extraction ";
String per = "%";
String cd = "Countdown ";
String col = ":";
String dot = ".";

long c1time = 1; //countdown time
long c2time = 1;
long c1stime = 1; //start time of countdown
long c2stime = 1;
int c1temp = 10; //current temp
int c2temp = 10;
int c1gtemp = 10; //goal temp
int c2gtemp = 10;
long c1ext = 0; //percent done
long c2ext = 0; 

bool c1run = false; //is cartrige running
bool c2run = false;

bool half;

long time, prevtime, clock, diff, sdiff;

int currentPage = 3;
int cc = 1;

LiquidCrystal_I2C lcd(0x27,20,4);
LiquidCrystal_I2C lcdt(0x21,20,4);

void setup()
{
  lcd.init();
  lcdt.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcdt.backlight();

  mylcd.Init_LCD();
  mylcd.Fill_Screen(WHITE); 

  mylcd.Set_Rotation(1); 
  mylcd.Set_Text_Mode(0);
  mylcd.Set_Text_colour(BLACK);
  mylcd.Set_Text_Back_colour(WHITE);
  mylcd.Set_Text_Size(2);

  Serial.begin(9600);
}

void start() {
  if(cc == 1) {
    c1run = true;
  }
  if(cc == 2) {
    c2run = true;
  }
}

void reset() {
  if(cc == 1) {
    c1run = false;
    c1ext = 0;
    c1time = c1stime;
  }
  if(cc == 2) {
    c2run = false;
    c2ext = 0;
    c2time = c2stime;
  }
}

void setTemp(int temp) {
  if(cc == 1) {
    c1gtemp = temp;
  }
  if(cc == 2) {
    c2gtemp = temp;
  }
}

void setTimer(int times) {
  if(cc == 1) {
    c1stime = times;
    c1time = times;
  }
  if(cc == 2) {
    c2stime = times;
    c2time = times;
  }
}


void loop()
{
  showMenu();
  run();
  display(1);
  display(2);
}

void run() {
  prevtime = time;
  time = millis();
  diff = time - prevtime;
  sdiff = diff / 1000;

  clock = clock + diff;

  if(clock > 1000 && !half) {
    if(c1run) {
      c1time--;
    }
    if(c2run) {
      c2time--;
    }
    half = true;
  }
  if(clock >= 2000) {
    if(c1run) {
      if(c1temp < c1gtemp) {
        c1temp = c1temp + 1;
      }
      if(c1temp > c1gtemp) {
        c1temp = c1temp - 1;
      }
      c1time--;
    }
    if(c2run) {
      if(c2temp < c2gtemp) {
        c2temp = c2temp + 1;
      }
      if(c2temp > c2gtemp) {
        c2temp = c2temp - 1;
      }
      c2time--;
    }
    clock = 0;
    half = false;
  }
  c1ext = ((c1stime - c1time) * 1000) / c1stime;
  c2ext = ((c2stime - c2time) * 1000) / c2stime;

  if(c1time <= 0) {
    reset();
  }
  if(c2time <= 0) {
    reset();
  }
}

void display(int cartrige) {
  if(cartrige == 1) {
    lcd.setCursor(0,0);
    String car = "Cartridge 1";
    lcd.print(car);

    lcd.setCursor(0,1);
    String c00 = temp + c1temp + c;
    lcd.print(c00);

    lcd.setCursor(0,2);
    int extP1 = c1ext / 10;
    int extP2 = c1ext % 10;
    String c01 = ext + extP1 + dot + extP2 + per;
    lcd.print(c01);

    lcd.setCursor(0,3);
    int m = c1time / 60;
    int s = c1time % 60;
    String c02 = cd + m + col + s;
    //int c02 = c1time;
    lcd.print(c02);
  }
  if(cartrige == 2) {
    lcdt.setCursor(0,0);
    String car = "Cartridge 2";
    lcdt.print(car);

    lcdt.setCursor(0,1);
    String c00 = temp + c2temp + c;
    lcdt.print(c00);

    lcdt.setCursor(0,2);
    int extP1 = c2ext / 10;
    int extP2 = c2ext % 10;
    String c01 = ext + extP1 + dot + extP2 + per;
    lcdt.print(c01);

    lcdt.setCursor(0,3);
    int m = c2time / 60;
    int s = c2time % 60;
    String c02 = cd + m + col + s;
    lcdt.print(c02);
  }
}

void showMenu() {
  char customKey = customKeypad.getKey();
  // Display the menu for the current page
  switch (currentPage) {
    case 0:
      mylcd.Print_String("Press A to Select Cartridge",15,15);
      mylcd.Print_String("Press B to Set Temperature",15,45);
      mylcd.Print_String("Press C to Set Timer",15,75);
      mylcd.Print_String("Press D to Start",15,105);
      mylcd.Print_String("Press * to Reset",15,135);
      switch (customKey) {
        case 'A':
          currentPage = 1;
          break;
        case 'B':
          currentPage = 2;
          break;
        case 'C':
          currentPage = 3;
          break;
        case 'D':
          start();
          break;
        case '*':
          reset();
          break;
      }
      break;
    case 1:
      // Menu options for page 1
      mylcd.Print_String("Select Cartridge 1 or 2",15,15);
      mylcd.Print_String("Press # to Return to Main Menu",15,300);
      switch (customKey) {
        case '1':
          cc = 1;
          currentPage = 0;
          break;
        case '2':
          cc = 2;
          currentPage = 0;
          break;
        case '#':
          currentPage = 0;
          break;
      }
      break;
    case 2:
      // Menu options for page 2
      static int tens = -1;
      static int ones = -1;

      String s = "";
      if (tens == -1) {
        s+="_";
      } else {
        s+=tens;
      }
      if (ones == -1) {
        s+="_";
      } else {
        s+=ones;
      }
      mylcd.Print_String("Set Temperature:"+s,15,15);
      mylcd.Print_String("15C - 40C",15,45);
      mylcd.Print_String("Press # to Return to Main Menu",15,300);

      if (customKey == '#') {
        currentPage = 0;
        ones = -1;
        tens = -1;
        break;
      } else if (customKey >= '0' && customKey <= '9') {
        if (tens == -1) {
          tens = customKey - '0';
        } else if (ones == -1) {
          ones = customKey - '0';
        } else {
          int temp = tens * 10 + ones;
          if (temp > 40) {
            temp = 40;
          }
          setTemp(temp);
          currentPage = 0;
          tens = -1;
          ones = -1;
        }
      }
      break;
    case 3:
      // Menu options for page 3
      static int minutes = -1;
      static int seconds = -1;
      String s2 = "";
      Serial.println("running");

      if (minutes == -1) {
        mylcd.Print_String("Set Minutes",15,15);
        mylcd.Print_String("0:00",15,45);
      } else {
        mylcd.Print_String("Set Seconds",15,15);
        s2+=minutes+":";
        if (seconds < 10) {
          s2+="0";
        }
        s2+=seconds;
        mylcd.Print_String(s2,15,45);
      }
      mylcd.Print_String("Press # to Return to Main Menu",15,300);

      if (customKey == '#') {
        currentPage = 0;
        minutes = -1;
        seconds = -1;
        break;
      } else if (customKey >= '0' && customKey <= '9') {
        if (minutes == -1) {
          minutes = customKey - '0';
          if (minutes > 5) {
            minutes = 5;
          }
        } else if (seconds == -1) {
          seconds = (customKey - '0') * 10;
        } else if (seconds % 10 == 0) {
          seconds += (customKey - '0');
        } else {
          if (seconds >= 60) {
            seconds = 59;
          }
          int totalSeconds = minutes * 60 + seconds;
          if (totalSeconds > 300) {
            totalSeconds = 300;
          }
          setTimer(totalSeconds);
          currentPage = 0;
          minutes = -1;
          seconds = -1;
        }
        break;
      }
      break;
  }
}
