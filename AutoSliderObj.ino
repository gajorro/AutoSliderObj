#include <Bounce2.h>
#include <AutoDriver.h>
#include <dSPINConstants.h>
#include <SPI.h>
#include <LiquidCrystal.h>
//include <avr/wdt.h>
//#define SMOOTHSTEP(x) ((x) * (x) * (3 - 2 * (x)))

//create autodriver board
AutoDriver boardA(10, 9);
//create LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//configure input pins
//configure button pins
//pin 8 for auto calibration pushbutton
//pin 1 for joystick pin
//pin A3 for resetting pin

const int calibrationPin = 8;
const int joystickPin = 1;
const int resetPin=A3;
//dlugosc slidera w krokach*microstep
const long length=185000;

//configure sensor pins
//joystick x pin
int sensorPin = A5;
//pot pin
int sensorPin2 = A2;

//output pins
int shutterPin = A0;

int canStart=1;
int menu=0;
int sensorValue = 0;
int sensorValue2 = 0;

double stepperSpeed = 0;
float MCstepperSpeed = 0;
float maxSpeed = 200;
int stepperAcc = 0;
int oldStepperAcc = 0;
int oldStepperSpeed = 0;

long oldPos = 0;

long maxPos = length;

int counterHelper = 0;

//timelapse variables
int timelapseTime=1;
int timelapseCount=100;
long timelapseInterval=1000;
long lastShot=0;
long lastShotTemp=0;
int timerHelper=0;
int currentPhotoNumber=0;

int allow_prepare=0;

//read joystick variables
long oldtime=0;
int first=1;
float joystickOutput=0;

int buttonState;
int buttonState2;
int resetState;
int bState;
const int ledPin = 13;      // the number of the LED pin
int joyState=0;
int firstrun=1;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

//AutoDriver boardB(14, 8);

String speedDisplay= "";
//menu
String menus[4] = { 
  "Timelapse", "Ruch swobodny", "Motion Control", "Motion Control 2"};

void setup()
{
  //  wdt_init();
  buttonState2=LOW;
  //joystick button
  pinMode(joystickPin, INPUT);
 
 //auto calibration push button
  pinMode(calibrationPin, INPUT);
  
  pinMode(resetPin, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Start?");
  lcd.setCursor(0, 1);
  //lcd.print(digitalRead(joystickPin));

  while (digitalRead(joystickPin))
  { 
    
  }
  delay(500);
  pinMode(ledPin, OUTPUT);
  pinMode(shutterPin, OUTPUT);
  allow_prepare=1;
}

void prepare()
{
  currentPhotoNumber=0;
  lcd.clear();
 // buttonState=debounce(calibrationPin);
  if (firstrun==1) {
  dSPINConfig(64,200,96);
  while (debounce(calibrationPin))
  {
    lcd.setCursor(0, 0);
    lcd.print("Kalibracja");
    //lcd.print("Calibration:");
    boardA.run(REV, 200);
    //CalibPin.update();
    //buttonState=debounce(calibrationPin);
  }
  lcd.clear();
  boardA.hardStop();
  boardA.resetPos();
  firstrun=0;  
}
else {
  setBasicParams(64,200,96);
       while (boardA.getPos()!=0)
        {
          boardA.goHome();
        }
}
  
  //lcd.clear();
  //lcd.print(boardA.getPos());
  
  while (!debounce(joystickPin))
  {
    sensorValue = analogRead(sensorPin);
    //joyState = map(sensorValue, 0, 1023, -64, 64);
    if (sensorValue<400)
    {
      menu = menu++;
      delay(250);
      if (menu>2)
      menu=0;
    }
    else if (sensorValue>=600)
    {
      menu = menu--;
      delay(250);
      if (menu<0)
      menu=2;
    }
    //else {menu=0; delay(250);}
    lcd.setCursor(0, 0);
    lcd.print("Wybierz tryb");
    lcd.setCursor(0, 1);
    lcd.print(menus[menu]+"         ");  
  }
  lcd.clear();
  delay(500);
  
  //reinitialize read joystick variables
  oldtime=0;
  first=1;
  joystickOutput=0;  
  setBasicParams(64,200,96);
  if (menu==0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Czas timelapse:");
    //lcd.print("Set timelapse time:");
    while(!debounce(joystickPin))
    {
      sensorValue = analogRead(sensorPin);
      timelapseTime=readJoystick(sensorValue,timelapseTime,0,240);
      lcd.setCursor(0, 1);
      lcd.print(String((int)timelapseTime)+" min    ");
      delay(150);
    }
    if (timelapseTime>2)
    setBasicParams(64,200,48);
    lcd.clear();
    lcd.setCursor(0, 0);
    delay(500);
    lcd.print("Liczba zdjec:");
    
  //reinitialize read joystick variables
  oldtime=0;
  first=1;
  joystickOutput=100;
    
    while (!debounce(joystickPin))
    {
      sensorValue = analogRead(sensorPin);    
      timelapseCount=readJoystick(sensorValue, timelapseCount,100,1000);
      lcd.setCursor(0, 1);
      lcd.print(String(timelapseCount)+"    ");
      delay(150);
    }
    if (timelapseTime==0)
      timelapseTime=1;

    stepperSpeed=calculateTimelapseSpeed(timelapseTime);
    timelapseInterval=calculateTimelapseInterval(timelapseTime,timelapseCount);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Interwal:");
    lcd.setCursor(0, 1);
    lcd.print((long)timelapseInterval);
    //delay(1000);

    while (!debounce(joystickPin))
    { 

    }
  }
  if (menu==2)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ustaw predkosc:");
    //set speed
    delay(500);
    
    //reinitialize read joystick variables
  oldtime=0;
  first=1;
  joystickOutput=0;
    
    while (!debounce(joystickPin))
    {
     sensorValue = analogRead(sensorPin);
     //invert values
     MCstepperSpeed=readJoystick(sensorValue, MCstepperSpeed,0,200);  
     lcd.setCursor(0, 1);
     lcd.print(String((int)MCstepperSpeed)+"    ");
     delay(200);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ustaw przyspieszenie:");
    //wait
    delay(500);
    //set acceleration
        //reinitialize read joystick variables
  oldtime=0;
  first=1;
  joystickOutput=0;
  while (!debounce(joystickPin))
    {
      sensorValue = analogRead(sensorPin);
      stepperAcc=readJoystick(sensorValue, stepperAcc,0,200);
      
      
        lcd.setCursor(0, 1);
        lcd.print(String(stepperAcc)+"     ");
        delay(150);
    } 
    lcd.clear();
    //if (MCstepperSpeed>150)
    if (MCstepperSpeed>20)
    dSPINConfig(stepperAcc,MCstepperSpeed,96);
    else
    dSPINConfig(stepperAcc,MCstepperSpeed,48);
    //else if 
    //dSPINConfig(stepperAcc,MCstepperSpeed,48);
  }  
}

float mapf(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void loop()
{
  //  long hlp=0;

  if (allow_prepare)
  {
    menu=0;
    prepare();
    allow_prepare=0;
  }


  //timelapse
  if (menu==0)
  {
    

    
    boardA.run(FWD, stepperSpeed);
    if (boardA.getPos()>maxPos)
    {
      stepperSpeed = 0;  
      //if (boardA.getPos()>maxPos)
      //{
        maxPos=boardA.getPos();
        boardA.setPos(maxPos);
        boardA.hardStop();
        setBasicParams(64,200,96);   
        //timelapseInterval=1;
      //}   
    }
    else {
      if (currentPhotoNumber<timelapseCount) 
      {
    if (millis()-lastShot>=timelapseInterval)
    {
      if (timerHelper<500)
      {
        if (timerHelper==0)
        {
          lastShotTemp=millis();
          
          currentPhotoNumber++;
          lcd.setCursor(0,0);
          lcd.print("Numer zdjecia");
          lcd.setCursor(0, 1);
          lcd.print((String)currentPhotoNumber+"/"+(String)timelapseCount+"    ");
        }
        timerHelper++;
        digitalWrite(shutterPin, HIGH);
      }
      else {
        timerHelper=0;
        lastShot=lastShotTemp;
        //digitalWrite(shutterPin, LOW);
      }
    }
    else {digitalWrite(shutterPin, LOW);
  }
  }
  else {
        lcd.clear();
        lcd.print("Koniec timelapse");
        lcd.setCursor(0, 1);
        while (!debounce(joystickPin))
        {
        allow_prepare=1;
        }
  }
    }
  
  }
  
  //free run
  if (menu==1)
  {
    if (menu==1)
    {
      MCstepperSpeed=200;
    }
    sensorValue2 = analogRead(sensorPin2);
    //stepperAcc = map(sensorValue2, 16, 1023, 1, 200);
    //invert values
    maxSpeed = map(sensorValue2, 0, 1023, 10, 200);
    
    /*if (abs(stepperAcc-oldStepperAcc)>2)
    {
      long tmp=boardA.getPos();
      dSPINConfig(stepperAcc, MCstepperSpeed);
      delay(500);
      oldStepperAcc=stepperAcc;
      boardA.setPos(tmp);
    }*/

    sensorValue = analogRead(sensorPin);
    stepperSpeed = speedOK(sensorValue);
      
    //lcd.clear();
    //lcd.print(stepperSpeed);

    if (stepperSpeed<-1)
    {
      boardA.run(REV, -stepperSpeed);
    }
    else if (stepperSpeed>1)
    {
      boardA.run(FWD, stepperSpeed);
    }
    else {
      boardA.run(FWD, 0);
      //delay(250);
      //boardA.hardStop();
    }
    if (boardA.getPos()<0||boardA.getPos()>maxPos)
    {
      stepperSpeed = 0;
      boardA.hardStop();
      if (boardA.getPos()<0)
      {
        boardA.resetPos();
      }
      else if (boardA.getPos()>maxPos)
      {
        maxPos=boardA.getPos();
        boardA.setPos(maxPos);
      }
    }
  }

  //motion control
  else if (menu==2) {
            delay(250);
    while (debounce(joystickPin))
    {   
      if (counterHelper==0)
      {
        maxPos=boardA.getPos();
        boardA.hardStop();
        lcd.clear();
        lcd.print("Pozycja koncowa");
        lcd.setCursor(0, 1);
        lcd.print(String(maxPos)+"      ");
      }
      else if (counterHelper==1) {
        while (boardA.getPos()!=0)
        {
          boardA.goHome();
        }
        canStart=0;
      }
      else {
        lcd.clear();
        lcd.print("Odtwarzam ruch");
        lcd.setCursor(0, 1);
        canStart=1; 
        counterHelper=0;
      }
      counterHelper++;
    }

    //else {
    if (canStart)
      boardA.goTo(maxPos);  
    //}
  }

  //if (menu==0)
  //{
  //lcd.setCursor(0,0);
  //lcd.print("Numer zdjecia");
  //lcd.setCursor(0, 1);
 // lcd.print((String)currentPhotoNumber+"/"+(String)timelapseCount+"    ");
  //}
  //else {
  if (menu==1)//||menu==2)
  {
    speedDisplay=String(boardA.getPos())+"         ";
  speedDisplay=String((int)stepperSpeed)+"         ";
  lcd.setCursor(0,0);
  lcd.print("Predkosc [k/s]:");
  lcd.setCursor(0, 1);
  lcd.print(speedDisplay);
  }
  //}


//resetState=digitalRead(resetPin);
if (!digitalRead(resetPin))
{
     lcd.setCursor(0, 1);
     lcd.print("RESET");
     delay(2000);
     allow_prepare=1; 
     maxPos=length;
     counterHelper=0;
     canStart=1;
}
}

bool debounce(int pin) 
{
  //there is difference beetween connection of normal button and joystick button
  //todo research;)
  
  Bounce debouncer = Bounce(); 
  debouncer.attach(pin);
  debouncer.update();
  debouncer.interval(5);
  return !debouncer.read();
}
int joystickControlValue()
{
  
}
//calculate speed from sensor value
double speedOK(int value)
{
  int speedSensorOutput = value;
  int speedOutput = 0;
  
  //map(speedSensorOutput, 29, 1023, -maxSpeed, maxSpeed);
  
  if (speedSensorOutput>527)
  {
  speedOutput=mapf(speedSensorOutput,528, 1023, 1, maxSpeed); 
  }
  else if (speedSensorOutput<523) 
  {
  speedOutput=mapf(speedSensorOutput,0, 522, -maxSpeed, -1);   
  }
  else speedOutput=0;
  
  //lcd.clear();
  //lcd.print(speedSensorOutput);
  /*if (abs(speedOutput-oldStepperSpeed)>2)
  {
    oldStepperSpeed=speedOutput;
   }
   else speedOutput=oldStepperSpeed;
   */
   //if (abs(speedOutput)>2)
   return speedOutput;
   //else return 0;
}
double calculateTimelapseSpeed(long tt)
{
  return (double)((length)/double(128*tt*60)); 
  //return 0.5;
}
long calculateTimelapseInterval(long tt, long amount)
{
  //return 600;
  return (long)((tt*60*1000)/amount);
}
float readJoystick(float inputValue, float currentOutput,float minValue, float maxValue)
{
  if (currentOutput>=minValue&&inputValue<=400&&currentOutput<maxValue)
      {
        if(first)
        {
        oldtime=millis();
        first=0;
        }
        if (millis()-oldtime>1000)
        {
        if (joystickOutput<maxValue-10)
        joystickOutput+=10;
        else joystickOutput=maxValue;
        //first=1;
        }
        else
        {
        joystickOutput++;
        }
        //first=1;
      }
      else if (currentOutput>minValue&&inputValue>=600&&currentOutput<=maxValue)
      {
        if(first)
        {
        oldtime=millis();
        first=0;
        }
        if (millis()-oldtime>1000)
        {
        if (joystickOutput>minValue+10)
        joystickOutput-=10;
        else joystickOutput=minValue;     
        //first=1;
        }
        else
        {
        joystickOutput--;
        }
        //first=1;
      }
      else first=1;
      return joystickOutput;
}







