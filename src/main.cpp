#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <WS2812Serial.h>
#include "MPU9250.h"

// GUItool: begin automatically generated code
AudioPlaySdWav playSdWav3;     //xy=154,392
AudioPlaySdWav playSdWav2;     //xy=159,355
AudioPlaySdWav playSdWav1;     //xy=384,2003
AudioMixer4 mixer1;            //xy=456,406
AudioOutputTDM tdm1;           //xy=558,2017
AudioOutputAnalogStereo dacs1; //xy=659,403
AudioConnection patchCord3(playSdWav2, 0, mixer1, 1);
AudioConnection patchCord4(playSdWav2, 1, mixer1, 0);
AudioConnection patchCord5(playSdWav1, 0, mixer1, 2);
AudioConnection patchCord6(playSdWav1, 1, mixer1, 3);
AudioConnection patchCord7(mixer1, 0, dacs1, 0);
AudioConnection patchCord8(mixer1, 0, dacs1, 1);
// GUItool: end automatically generated code

#define onOffButton PIN_A13
#define colorButton PIN_A15
#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11 // not actually used
#define SDCARD_SCK_PIN 13  // not actually used
#define PIN 1
//Colors
#define OFFCOLOR 0x000000
#define GREEN 0x17ff00
#define RED 0xFF0000
#define BLUE 0x12e3f0
#define PURPLE 0x9c11cc
#define PINK 0xe359c7
#define YELLOWORANGE 0xef8d0f
#define WHITE 0xffffff

const int numled = 144;
int previousMillisInterrupt = 0;
int previousMillisAccel = 0;
int currentMillisAccel = 0;
int xSwingThresholdPositive = 3;
int zSwingThresholdPositive = 3;
int xSwingThresholdNegative = -3;
int zSwingThresholdNegative = -3;
int clashThreshold = 28;
int clashThresholdNegative = -28;
int colorFlag = 0;
//How many colors do we have, with the exception of OFF
const int NUMOFCOLORS = 7;
int colorInUse = GREEN; //Green by default

int function = 0; //what action are we going to perform
int status = 0;   //global status int we can use for initializations and usages

byte drawingMemory[numled * 3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled * 12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, PIN, WS2812_GRB);
MPU9250 IMU(Wire, 0x68);

void buttonPress()
{
  unsigned long currentMillisInterrupt = millis();
  if (currentMillisInterrupt - previousMillisInterrupt >= 1000)
  {
    function++;
    Serial.print("Function is now: ");
    Serial.println(function);

    if (function >= 4)
    {
      function = 0;
    }
    previousMillisInterrupt = currentMillisInterrupt;
  }
}

void switchColors()
{
  byte colorByte = 0;

  Serial.println("Switching colors");

  colorFlag++;
  if (colorFlag > NUMOFCOLORS)
  {
    colorFlag = 0;
  }
  colorByte = 1;

  switch (colorFlag)
  {
    //GREEN
  case 0:
    if (colorByte == 1)
    {
      for (int i = 1; i < numled; i++)
      {
        leds.setPixel(i, GREEN);
        leds.setPixel(i + 1, GREEN);
        i++;
        leds.show();
      }
      colorInUse = GREEN;
      colorByte = 0;
    }
    break;
    //BLUE
  case 1:
    if (colorByte == 1)
    {
      for (int i = 1; i < numled; i++)
      {
        leds.setPixel(i, BLUE);
        leds.setPixel(i + 1, BLUE);
        i++;
        leds.show();
      }
      colorInUse = BLUE;
      colorByte = 0;
    }
    break;
    //RED
  case 2:
    if (colorByte == 1)
    {
      for (int i = 1; i < numled; i++)
      {
        leds.setPixel(i, RED);
        leds.setPixel(i + 1, RED);
        i++;
        leds.show();
      }
      colorInUse = RED;
      colorByte = 0;
    }
    break;
    //PURPLE
  case 3:
    if (colorByte == 1)
    {
      for (int i = 1; i < numled; i++)
      {
        leds.setPixel(i, PURPLE);
        leds.setPixel(i + 1, PURPLE);
        i++;
        leds.show();
      }
      colorInUse = PURPLE;
      colorByte = 0;
    }
    break;
    //PINK
  case 4:
    if (colorByte == 1)
    {
      for (int i = 1; i < numled; i++)
      {
        leds.setPixel(i, PINK);
        leds.setPixel(i + 1, PINK);
        i++;
        leds.show();
      }
      colorInUse = PINK;
      colorByte = 0;
    }
    break;
  case 5:
    if (colorByte == 1)
    {
      for (int i = 1; i < numled; i++)
      {
        leds.setPixel(i, YELLOWORANGE);
        leds.setPixel(i + 1, YELLOWORANGE);
        i++;
        leds.show();
      }
      colorInUse = YELLOWORANGE;
      colorByte = 0;
    }
    break;
  case 6:
    if (colorByte == 1)
    {
      for (int i = 1; i < numled; i++)
      {
        leds.setPixel(i, WHITE);
        leds.setPixel(i + 1, WHITE);
        i++;
        leds.show();
      }
      colorInUse = WHITE;
      colorByte = 0;
    }
    break;
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.println("Setup start");
  Serial.begin(9600);

  //Set LEDs to Off
  leds.begin();
  for (int i = numled; i > -10; i--)
  {
    leds.setPixel(i, OFFCOLOR);
    leds.show();
  }
  leds.show();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(onOffButton, INPUT);
  pinMode(colorButton, INPUT);
  delay(250);

  // start communication with IMU
  status = IMU.begin();
  if (status < 0)
  {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1)
    {
    }
  }

  AudioMemory(8);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  mixer1.gain(0, 0.1f);
  mixer1.gain(1, 0.1f);

  attachInterrupt(digitalPinToInterrupt(onOffButton), buttonPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(colorButton), switchColors, FALLING);

  Serial.println(playSdWav1.isPlaying() ? "TRUE" : "FALSE");
  if (playSdWav1.isPlaying() == false)
  {
    Serial.println("Playing startup sound");
    playSdWav1.play("SETUP.WAV");
    delay(1000); // wait for library to parse WAV info
  }

  Serial.println("Setup complete");
}

//Empty, because it stays off (I'd unplug the battery here to save power)
void stayOff() {}

void turnOn()
{
  digitalWrite(LED_BUILTIN, HIGH);

  if (playSdWav1.isPlaying() == false)
  {
    Serial.println("Playing on sound");
    playSdWav1.play("POWERON.WAV");
    //Seems this value is dependent on how long my ignition sound is
    delay(10);
    // Don't play sound here. It overrides the ignition for some reason
    //  playSdWav2.play("HUM.WAV");
    //  delay(10);
  }

  for (int i = 1; i < numled; i++)
  {
    leds.setPixel(i, colorInUse);
    leds.setPixel(i + 1, colorInUse);
    i++;
    leds.show();
  }

  function = 2;
}

void turnOff()
{
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Playing off sound");
  playSdWav1.play("POWEROFF.WAV");
  delay(10);

  for (int i = numled; i > -10; i--)
  {
    leds.setPixel(i, OFFCOLOR);
    leds.show();
  }

  function = 0;
}

void swing()
{
  int index = random(0, 15);

  const char *swing = "";

  switch (index)
  {
  case 0:
    swing = "SWING1.WAV";
    break;
  case 1:
    swing = "SWING2.WAV";
    break;
  case 2:
    swing = "SWING3.WAV";
    break;
  case 3:
    swing = "SWING4.WAV";
    break;
  case 4:
    swing = "SWING5.WAV";
    break;
  case 5:
    swing = "SWING6.WAV";
    break;
  case 6:
    swing = "SWING7.WAV";
    break;
  case 7:
    swing = "SWING8.WAV";
    break;
  case 8:
    swing = "SWING8.WAV";
    break;
  case 9:
    swing = "SWING10.WAV";
    break;
  case 10:
    swing = "SWING11.WAV";
    break;
  case 11:
    swing = "SWING12.WAV";
    break;
  case 12:
    swing = "SWING13.WAV";
    break;
  case 13:
    swing = "SWING14.WAV";
    break;
  case 14:
    swing = "SWING15.WAV";
    break;
  case 15:
    swing = "SWING16.WAV";
    break;
  default:
    swing = "SWING1.WAV";
    break;
  }

  if (playSdWav2.isPlaying() == false)
  {
    Serial.print("Playing swing sound: ");
    Serial.println(swing);
    playSdWav1.play(swing);
    delay(10);
  }
}

void clash()
{
  int index = random(0, 2);
  const char *clash = "";

  switch (index)
  {
  case 0:
    clash = "CLASH1.WAV";
    break;
  case 1:
    clash = "CLASH2.WAV";
    break;
  case 2:
    clash = "CLASH3.WAV";
    break;
  }

  if (playSdWav2.isPlaying() == false)
  {
    Serial.println("Playing clash sound");
    playSdWav1.play(clash);
    delay(10);
  }
}

void readAccelerometer()
{
  IMU.readSensor();

  float gyroX = IMU.getGyroX_rads();
  float gyroZ = IMU.getGyroZ_rads();
  float accelerationTotal = IMU.getAccelX_mss() + IMU.getAccelY_mss() + IMU.getAccelZ_mss();

  // Serial.print("Accelertaion Total: " );
  // Serial.println(accelerationTotal, 6);

  if (accelerationTotal >= clashThreshold || accelerationTotal <= clashThresholdNegative)
  {
    Serial.println("We've clashed");
    clash();
  }

  if (gyroX >= xSwingThresholdPositive || gyroX <= xSwingThresholdNegative)
  {
    Serial.println("SWINGING!");
    swing();
  }
  if (gyroZ >= zSwingThresholdPositive || gyroZ <= zSwingThresholdNegative)
  {
    Serial.println("SWINGING!");
    swing();
  }

  delay(100);
}

void poweredOn()
{
  if (playSdWav1.isPlaying() == false)
  {
    Serial.println("Playing HUM sound");
    playSdWav1.play("HUM.WAV");
    delay(10);
  }

  readAccelerometer();
}

void loop()
{

  // switchColors();
  // put your main code here, to run repeatedly:
  switch (function)
  {
  case 0:
    stayOff();
    break;
  case 1:
    turnOn();
    break;
  case 2:
    poweredOn();
    break;
  case 3:
    turnOff();
    break;
  default:
    break;
  }
}