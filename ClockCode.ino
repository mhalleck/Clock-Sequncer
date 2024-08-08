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

int BPM = 120;
//holds how many milliseconds = beat
unsigned long beatDelay = 1;
int totalBeats = 16;
unsigned long cTime;
unsigned long lastBeat = 0;
long counter = -1; //not used anymore?

bool beat = false; //not used anymore?
bool sleep = false;
bool sleepConfirm = false;
bool wasSleep = false;
bool needUpdate = true;

bool clockState1 = LOW; 
bool clockState2 = LOW; 
bool clockState3 = LOW;
bool clockState4 = LOW;
bool clockState5 = LOW;
bool clockState6 = LOW;
bool clockState7 = LOW;
bool clockState8 = LOW;


int led1 = 8; //this pulses every tick
int led2 = 11; //this pulses every full beat'
int led3 = 4;
int led4 = 10;
int led5 = 3;
int led6 = 17;
int led7 = 2;
int led8 = 12;

String nextNum = "";
int nextNumLength = 0;
bool changeSequence = false;
bool changeDivision = false;
int microSequence = 0;


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

int sequence = 0;
int displaySeq = 0;
bool seqChange = false;
int sequences[12][7] = {
  {2, 4, 8, 16, 32, 64, 128},          {3, 5, 7, 9, 11, 13, 15},        {2, 3, 4, 5, 6, 7, 8},                     {3, 5, 8, 13, 21, 34, 55},
  {2, 3, 5, 7, 11, 13, 17},            {3, 6, 10, 15, 21, 28, 36},      {4, 9, 16, 25, 36, 49, 64},                {4, 10, 20, 35, 56, 84, 120},
  {5, 14, 30, 55, 91, 140, 204},       {8, 27, 64, 125, 216, 343, 512}, {32, 243, 1024, 3125, 7776, 16807, 32768}, {13, 37, 73, 121, 181, 253, 337}
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

  //declares our LED pins as being for output
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);
  digitalWrite(led7, LOW);
  digitalWrite(led8, LOW);

  
  
  lcd.init();       
  // Print a message to the LCD.
  lcd.backlight();

  //sets in X, y
  lcd.setCursor(0,0);
  lcd.print("________________");
  lcd.setCursor(0,1);
  lcd.print("                ");

  //initial setup for interrupt
  //calculate initial beat delay, this is in clock ticks / 64, so multiply by 64 for the actual number of clock ticks, 250 is the ticks *  64 that equals 1 ms
  //first figure out how many miliseconds per 16/th
  beatDelay = 250.0 * 1000.0 * 60.0 / BPM / totalBeats;
  Serial.println(beatDelay);
  
  TCCR1A = 0;           // Init Timer1A
  TCCR1B = 0;           // clears any prescale value, and sets clock to off
  TCCR1B |= B00000011;  // turns clock on with prescale value of 64
  OCR1A = beatDelay;        // sets initial tick delay value for COMPA interrupt
  TIMSK1 |= B00000010;  // Enable Timer COMPA Interrupt
}


void loop()
{
  //section for running clock
 
  //T0
  cTime = micros();
   
        
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
//function == 1
void BPMAdjust()
{

  if (pressed_butt1) //increase (right)
  {
    needUpdate = true;
    //needs to cap BPM otherwise it tries to beat so fast that no other code ever executes
    if (BPM <= 300)
    {
      if (coarse)
        BPM += 5;
      else
        BPM++;
    }
      
    //calculates clock delay in clock ticks, 250 ticks = 1 ms
    beatDelay = 250.0 * 1000.0 * 60.0 / BPM / totalBeats;
    //the beatDelay is what the interrupt is adding to the compare counter, so it will always be updated properly loooool
  }
  else if (pressed_butt2) //decrease (left)
  {
    needUpdate = true;

    if (BPM > 15)
    {
      if (coarse)
        BPM -= 5;
      else
        BPM--;
    }

    //calculates clock delay in clock ticks, 250 ticks = 1 ms
    beatDelay = 250.0 * 1000.0 * 60.0 / BPM / totalBeats;
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
        needUpdate = true;
  }

  
  



  //screen refresh section
      if (millis() - lastRefresh > 50 && needUpdate && function == 1){
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
//function == 2
void divisionSelect()
{
    //for the purposes of this code, needUpdate will be used for updating which sequence is displayed
    //change division goes true when we hit enter on a division number so we can increase or decrease it
    //microSequence controls which of the divisions we've selected to change, we only have 7 so it wraps at 7
    
    if (pressed_butt1) //right
    {
      
      if (changeDivision)
      {
        if (coarse)
          sequences[displaySeq][microSequence] += 5;
        else
          sequences[displaySeq][microSequence]++;
      }
      else if (changeSequence)
      {
        microSequence++;
        microSequence = microSequence % 7;
      }
      else
      {
        displaySeq++;
      }
      
      needUpdate = true;
    }
    else if (pressed_butt2) // left
    {
      if (changeDivision)
      {
        if (coarse)
        {
            if (sequences[displaySeq][microSequence] > 6)
              sequences[displaySeq][microSequence] -= 5;
            else
              sequences[displaySeq][microSequence] = 1;
        }
        else
        {
          if (sequences[displaySeq][microSequence] > 1)
            sequences[displaySeq][microSequence]--;
        }
      }
      else if (changeSequence)
      {
        if (microSequence == 0)
          microSequence = 7;
        microSequence--;
      }
      else
      {
        if (displaySeq == 0)
          displaySeq = 12;
        displaySeq--;
      }
      needUpdate = true;
    }
    else if (pressed_butt3) // select
    { 
      change = true;
      needUpdate = true;
      
      //tells our code that we need to reset the LEDs all to high, minus the beat drive
      //seqChange = true;

      //we only want to do this when we move between sequences, if this isn't the case that means we're currently on the sequence we want to change the divisions of
      if (sequence != displaySeq)
      {
        sequence = displaySeq;

        //tells the screen to fill in the dot
        change = true;
  
        //this resets the counter, sets all LEDs to off, and sets the clockstates back to low
        counter = -1;
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        digitalWrite(led4, LOW);
        digitalWrite(led5, LOW);
        digitalWrite(led6, LOW);
        digitalWrite(led7, LOW);
        digitalWrite(led8, LOW);
        
        clockState1 = LOW;
        clockState2 = LOW;
        clockState3 = LOW;
        clockState4 = LOW;
        clockState5 = LOW;
        clockState6 = LOW;
        clockState7 = LOW;
        clockState8 = LOW;
      }
      else if (changeDivision) //if changeDivision is true that means we want to change the value, if we hit enter that means we are done changing the value
      {
        //flips the value of coarse
        coarse = !coarse;
        
      }
      else if (changeSequence)
      {
        changeDivision = true;
      }
      else //if we hit here, that means change division and change sequence are both false, which means we have selected the sequence we want to change, it is the currently running sequence
      {
        changeSequence = true;
      }

    }

    
    //for thhis loop the function key will back us out of changing the division/sequence
    if (pressed_butt4)
    {
        
        if (changeDivision)
        {
          needUpdate = true;
          changeDivision = false;
        }
        else if (changeSequence)
        {
          needUpdate = true;
          changeSequence = false;
        }
        else
        {
          function++;
          function = function % 4;
          displaySeq = sequence;
          needUpdate = true;
        }
    }

    //wraps around displaySeq value when it reaches 24
    displaySeq = displaySeq % 12;


  //screen refresh section
      if (function == 2 && (needUpdate || change)){
          
          //Refresh Display

          if (needUpdate)
          {

              //depending on whether we are in a division change or a sequence value change
              //when we add the number to redraw the screen we want to do something to it
              topLine = "";
              bottomLine = "";

              //outline for procedurally building topline and bottom line from numbers in sequence
              for (int i = 0; i < 7; i++) 
              {
                
                if (changeDivision)
                {
                  if (i == microSequence)
                    nextNum = ">" + String(sequences[displaySeq][i]) + "<";
                  else
                    nextNum = String(sequences[displaySeq][i]);
                    
                }
                else if (changeSequence)
                {
                  if (i == microSequence)
                    nextNum = "_" + String(sequences[displaySeq][i]) + "_";
                  else
                    nextNum = String(sequences[displaySeq][i]);
                }
                else
                  nextNum = String(sequences[displaySeq][i]);


                //sets nextnum length value
                if (topLine.length() == 0)
                  nextNumLength = nextNum.length();
                else
                  nextNumLength = nextNum.length() + 2;
                
                if (topLine.length() + nextNumLength <= 16)
                {
                  //if it's the first number we don't want to add a space and a comma
                  if (topLine.length() == 0)
                    topLine = nextNum;
                  else
                    topLine = topLine + ", " + nextNum;
                }
                else
                {
                  while (topLine.length() < 16)
                    topLine = topLine + " ";
                  if (bottomLine.length() == 0)
                    bottomLine = nextNum;
                  else
                    bottomLine = bottomLine + ", " + nextNum;
                }
              }

              //filles the rest of the lines with blanks so we don't have characters carrying over from last write
              while (bottomLine.length() < 16)
                bottomLine = bottomLine + " ";


    
              if (sequence == displaySeq)
              {
                if (changeDivision)
                  if (coarse)
                    bottomLine[15] = 'C';
                  else
                    bottomLine[15] = 'F';
                else if (changeSequence)
                  bottomLine[15] = 'S';
                else
                  bottomLine[15] = '*';
              }
              else
              {
                bottomLine[15] = 'o';
              }
  
              lcd.setCursor(0,0);
              lcd.print(topLine);
              lcd.setCursor(0,1);
              lcd.print(bottomLine);
            }

            //change loop is only for changing the 'o' to a '*'
            if (change)
            {
              lcd.setCursor(15,1);
              if (changeDivision)
              {
                if (coarse)
                  lcd.print('C');
                else
                  lcd.print('F');
              }
              else if (changeSequence)
              {
                lcd.print('S');
              }
              else
              {
                lcd.print('*');
              }
            }
  
            needUpdate = false;
            change = false;
        }
  
}

//////////////////////////////////////////////////////////////////////////////////////
//function == 3
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
          needUpdate = true;
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
          TCCR1B |= B00000011;  // turns clock on with prescale value of 64
          needclear = true;
          needUpdate = true;
      }

      if (sleepConfirm)
      {
          lcd.noBacklight();
          lcd.noDisplay();
          TCCR1B = 0;           // clears any prescale value, and sets clock to off
      }
  
  //screen refresh section
      if (millis() - lastRefresh > 50 && needUpdate && function == 3){
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
//function == 0
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
        needUpdate = true;
      }

      
      //refreshes the screen, changes based on button pressed
      if (millis() - lastRefresh > 50 && needUpdate && function == 0){
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

      change = false;

  
}


ISR(TIMER1_COMPA_vect)
{
  OCR1A += beatDelay; // Advance The COMPA Register
  // Handle The Timer Interrupt
  //...
  counter++;
  clockPulse();
}

void clockPulse()
{

      
      clockState1 = !clockState1;
      digitalWrite(led1, clockState1);

      
      //controls beat light
      if (counter % sequences[sequence][0] == 0)
      {
        
        clockState2 = !clockState2;
        digitalWrite(led2, clockState2);
      }
      
      //controls beat light
      if (counter % sequences[sequence][1] == 0)
      {
        
        clockState3 = !clockState3;
        digitalWrite(led3, clockState3);
      }
      
      //controls beat light
      if (counter % sequences[sequence][2] == 0)
      {
        
        clockState4 = !clockState4;
        digitalWrite(led4, clockState4);
      }
      
      //controls beat light
      if (counter % sequences[sequence][3] == 0)
      {
        
        clockState5 = !clockState5;
        digitalWrite(led5, clockState5);
      }
      
      //controls beat light
      if (counter % sequences[sequence][4] == 0)
      {
        
        clockState6 = !clockState6;
        digitalWrite(led6, clockState6);
      }
      
      //controls beat light
      if (counter % sequences[sequence][5] == 0)
      {
        
        clockState7 = !clockState7;
        digitalWrite(led7, clockState7);
      }
      
      //controls beat light
      if (counter % sequences[sequence][6] == 0)
      {
        
        clockState8 = !clockState8;
        digitalWrite(led8, clockState8);
      }


  
}
