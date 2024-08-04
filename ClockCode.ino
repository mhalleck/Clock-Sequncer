//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


int button_state;
int last_state1 = HIGH;
int last_state2 = HIGH;
int last_state3 = HIGH;
int last_state4 = HIGH;

int x = 0;

bool pressed;
int pressed_butt1;
int pressed_butt2;
int pressed_butt3;
int pressed_butt4;
int function = 0;

bool coarse = false;

float BPM = 120.0;
//holds how many milliseconds = beat
float beatDelay = 1.0;
int totalBeats = 16;
unsigned long cTime;
unsigned long lastBeat = 0;
long counter = -1;
bool beat = false;
bool sleep = false;
bool sleepConfirm = false;
bool wasSleep = false;
bool needUpdate = true;

bool clockState1 = HIGH; 
bool clockState2 = HIGH; 


bool change = false;
bool needclear = false;


unsigned long debounceDuration = 20; // millis
unsigned long lastTimeButtonStateChanged = 0;
int lastLEDTimeout = 100;

unsigned long lastRefresh = 0;

const int button1 = 5;
const int button2 = 6;
const int button3 = 7;
const int button4 = 9;

int lastLED = 0;
bool LED = true;


int beats[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const static int sequences[24][7] = {
  {2, 4, 8, 16, 32, 64, 128},          {3, 5, 7, 9, 11, 13, 15},        {2, 3, 4, 5, 6, 7, 8},                     {3, 5, 8, 13, 21, 34, 55},
  {2, 3, 5, 7, 11, 13, 17},            {3, 6, 10, 15, 21, 28, 36},      {4, 9, 16, 25, 36, 49, 64},                {4, 10, 20, 35, 56, 84, 120},
  {5, 14, 30, 55, 91, 140, 204},       {8, 27, 64, 125, 216, 343, 512}, {32, 243, 1024, 3125, 7776, 16807, 32768}, {13, 37, 73, 121, 181, 253, 337},
  {14, 51, 124, 245, 426, 679, 1016},  {2, 4, 8, 12, 24, 48, 72},       {16, 22, 34, 36, 46, 56, 64},              {72, 108, 200, 288, 392, 432, 500},
  {6, 21, 28, 301, 325, 496, 697},     {2, 8, 20, 28, 50, 82, 126},     {21, 33, 57, 69, 77, 93, 129},             {2, 3, 2, 5, 6, 7, 2},
  {30, 42, 66, 70, 78, 102, 105},      {9, 45, 55, 99, 297, 703, 999},  {70, 836, 4030, 5830, 7192, 7912, 9272},   {15, 34, 65, 111, 175, 260, 369},
};



String topLine;
String bottomLine;

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//D pins, should be d5

void setup()
{
    Serial.begin(9600);
    
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(10, OUTPUT);
  pinMode(4, OUTPUT);

  
  
  lcd.init();       
  // Print a message to the LCD.
  lcd.backlight();

  //sets in X, y
  lcd.setCursor(0,0);
  lcd.print("________________");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lastBeat = micros();
}


void loop()
{
  //section for running clock
 
  //T0
  cTime = micros();

  //this gives the number of milliseconds we need for each sub-beat (1/16th of a beat)
  //if that much time has passed then we know it's time to run the beat again

  
  //now calculate microseconds
  beatDelay = 1000.0 * 1000.0 * 60.0 / BPM / totalBeats;

  //we are running the LED for about half the total beat time
  lastLEDTimeout = beatDelay * 4;

  //counts beats, pulses, each beat is 1/16th time of bpm
  //this is a global advance that will not reset, depending on our mode will change how this triggers
  if ((cTime - lastBeat) >= beatDelay)
  {
      //Serial.println(millis());
      //this prints how many milliseconds ahead this beat is
      Serial.print("P: ");
      Serial.println((((cTime - lastBeat)) - beatDelay));
      beat = true;
      lastBeat = lastBeat + beatDelay;
      //lastBeat = cTime;
      counter++;


      clockState1 = !clockState1;
      digitalWrite(4, clockState1);

      
      //controls beat light
      if (counter % 8 == 0)
      {
        
        //digitalWrite(4, HIGH);
        //LED = true;
        clockState2 = !clockState2;
        digitalWrite(10, clockState2);
      }
      
  }
  else
  {
    beat = false;
  }
   
        
   //if (pressed)
   {
      //pressed = false;

   
   //button checks
   if (millis() - lastTimeButtonStateChanged > debounceDuration) 
   {
       lastTimeButtonStateChanged = millis();
          
       button_state = digitalRead(button1);
       if(last_state1 == LOW && button_state == HIGH)
       {
         pressed_butt1 = true;
       }
       last_state1 = button_state;

       
       button_state = digitalRead(button2);
       if(last_state2 == LOW && button_state == HIGH)
       {
         pressed_butt2 = true;
       }
       last_state2 = button_state;


       
       button_state = digitalRead(button3);       
       if(last_state3 == LOW && button_state == HIGH)
       {
         pressed_butt3 = true;

         //these two lines turn light on
         //digitalWrite(10, HIGH);
         //lastLED = millis();  
       }
       last_state3 = button_state;


       button_state = digitalRead(button4);
       if(last_state4 == LOW && button_state == HIGH)
       {
         Serial.println("pressed 4");
         pressed_butt4 = true;
         needclear = true;
         needUpdate = true;
         
         Serial.println(function);
         //function += 1;
         Serial.println(function);
         Serial.println(beatDelay);
         Serial.println(counter);

          
         //allows for 4 function states, 0, 1, 2, 3
         if (function == 4)
          function = 0;

         //these two lines turn light on
         //digitalWrite(10, HIGH);
         //lastLED = millis();  
       }
       last_state4 = button_state;

   }
      
    //calls the sequencer code
      if (function == 0)
      {
        sequencer();
      }
      else if (function == 1)
      {
        BPMAdjust();
      }
      else if (function == 2)
      {
        divisionSelect();
      }
      else if (function == 3)
      {
        sleepMode();
      }
      
      //at end we wanna clear val
      pressed_butt1 = false;
      pressed_butt2 = false;
      pressed_butt3 = false;
      pressed_butt4 = false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//first routine
void BPMAdjust()
{

  if (pressed_butt1) //increase (right)
  {
    needUpdate = true;
    if (coarse)
      BPM += 5;
    else
      BPM++;
  }
  else if (pressed_butt2) //decrease (left)
  {
    needUpdate = true;
    if (coarse)
      BPM -= 5;
    else
      BPM--;
  }
  else if (pressed_butt3) //select button, changes from coarse to fine
  {
    needUpdate = true;
    if (coarse)
      coarse = false;
    else
      coarse = true;
  }
  
  if (pressed_butt4) //function select button
  {
        function++;
        function = function % 4;
  }
  



  //screen refresh section
      if (millis() - lastRefresh > 50 && needUpdate && beat){
          //Refresh Display
          lastRefresh = millis();
          needUpdate = false;

          if (needclear)
          {
            needclear = false;
            //lcd.setCursor(0,0);
            //lcd.print("                ");
            lcd.clear();
          }
          lcd.setCursor(0,0);
          lcd.print("BPM: " + String(BPM) + "    ");
          //lcd.setCursor(5,0);
          //lcd.print(BPM);

          
          lcd.setCursor(0,1);
          if (coarse)
          {
            lcd.print("Coarse (5)       ");
          }
          else
          {
            lcd.print("Fine (1)         ");
          }
          
      }
}

//////////////////////////////////////////////////////////////////////////////////////
//Second routine
void divisionSelect()
{
    if (pressed_butt1)
    {
      
    }
    else if (pressed_butt2)
    {
      
    }
    else if (pressed_butt3)
    {
      
    }

    

    if (pressed_butt4)
    {
        function++;
        function = function % 4;
    }


  //screen refresh section
      if (millis() - lastRefresh > 50 && needUpdate && beat){
          
          needUpdate = false;
          //Refresh Display
          
          lastRefresh = millis();
          lcd.setCursor(0,0);
          lcd.print("________________");
          lcd.setCursor(0,1);
    
          bottomLine = "****************";
          //Serial.println(x);
          //bottomLine.setCharAt(x, 'T');
          bottomLine[x] = 'T';
          lcd.print(bottomLine);
      }
  
}

//////////////////////////////////////////////////////////////////////////////////////
//Third Routine
void sleepMode()
{
      
      if (pressed_butt1) //increase (right)
      {
         //tells the screen we are selecting sleep
         sleep = true;
         needUpdate = true;
         
         //wakes us up on button press
         if (sleepConfirm)
         {
          sleepConfirm = false;
          wasSleep = true;
         }
      }
      else if (pressed_butt2) //decrease (left)
      {
        
        needUpdate = true;
        sleep = false;
         if (sleepConfirm)
         {
          sleepConfirm = false;
          wasSleep = true;
         }
      }
      else if (pressed_butt3) //select button
      {
         if (sleepConfirm)
         {
          sleepConfirm = false;
          wasSleep = true;
          sleep = false;
         }
         
        //if sleep is selected, initiate the code to put us to sleep
        if (sleep)
          sleepConfirm = true;

      }
      else if (pressed_butt4) //function select button, deprecated
      {
          function++;
          function = function % 4;
         if (sleepConfirm)
         {
          sleepConfirm = false;
          wasSleep = true;
         }
      }

      if (wasSleep)
      {
          wasSleep = false;
                  
          lcd.init();       
          // Print a message to the LCD.
          lcd.backlight();
      }

      if (sleepConfirm)
      {
          lcd.noBacklight();
          lcd.noDisplay();
      }
  
  //screen refresh section
      if (millis() - lastRefresh > 50 && needUpdate && beat){
          //Refresh Display
          lastRefresh = millis();
          needUpdate = false;

          if (needclear)
          {
            needclear = false;
            
            //lcd.setCursor(0,0);
            //lcd.print("                ");
            //lcd.setCursor(0,1);
            //lcd.print("                ");
            lcd.clear();
            
            lcd.setCursor(0,0);
            lcd.print("Sleep? ");
          }
          
          lcd.setCursor(0,1);
          //lcd.print("<NO>  YES");

          lcd.setCursor(0,1);
          if (sleep)
          {
            lcd.print(" NO  <YES>");
          }
          else
            lcd.print("<NO>  YES ");
          
          
      }
}

//////////////////////////////////////////////////////////////////////////////////////
//Fourth Routine
void sequencer()
{
  
   //what we do with the pressed button results
      if (pressed_butt1)
      {
        needUpdate = true;
        if (x < 15)
        x++;
        //if (BPM < 200)
          //BPM++;
      }
      
      if (pressed_butt2)
      {
        needUpdate = true;
        if( x > 0)
        x--;
        //if (BPM > 1)
          //BPM--;
      }
      
      if (pressed_butt3)
      {
        needUpdate = true;
        change = true;
        lcd.setCursor(x,0);
        if (beats[x] == 1)
        {
          beats[x] = 0;
          lcd.print("_");
        }
        else if (beats[x] == 0)
        {
          beats[x] = 1;
          lcd.print("X");
        }
      }

      if (pressed_butt4)
      {
        function++;
        function = function % 4;
      }

      
      //refreshes the screen, changes based on button pressed
      if (millis() - lastRefresh > 50 && needUpdate && beat){
          //Refresh Display
          lastRefresh = millis();

          if (needclear)
          {
            needclear = false;
            lcd.setCursor(0,0);
            topLine = "________________";

            //if we're reselecting this we need to rebuild the top line from 'memory'
            for (int i = 0; i < 16; i++)
            {
              if (beats[i] == 1)
              {
                topLine[i] = 'X';
              }
            }
            
            lcd.print(topLine);
          }
          
          lcd.setCursor(0,1);
          
          bottomLine = "****************";
          //Serial.println(x);
          //bottomLine.setCharAt(x, 'T');
          bottomLine[x] = 'T';
          lcd.print(bottomLine);
          //Serial.println(function);
      }

  
}
