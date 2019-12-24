#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <OctoWS2811.h>
// #include <WS2812Serial.h>
// #include <Adafruit_LSM9DS1.h>
// #include <Adafruit_Sensor.h>  // not used in this demo but required!

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

#define onOffButton PIN_A14
#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11 // not actually used
#define SDCARD_SCK_PIN 13  // not actually used

const int ledsPerStrip = 120;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB || WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);


int function = 0; //what action are we going to perform
int previousMillisInterrupt = 0;

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

void setup()
{
  // put your setup code here, to run once:
  Serial.println("Setup start");
  Serial.begin(9600);

   leds.begin();
  leds.show();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(onOffButton, INPUT);

  AudioMemory(20);
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
    delay(1400);
    playSdWav1.play("HUM.WAV");
    delay(10);
  }

  for(int i = 0; i < ledsPerStrip; i++){
    leds.setPixel(i, 255,255,255);
  }

  leds.show();

  function = 2;
}

void turnOff()
{
  digitalWrite(LED_BUILTIN, LOW);

    Serial.println("Playing off sound");
    for(int i = 0; i < ledsPerStrip; i++){
    leds.setPixel(i, 0,0,0);
  }

  leds.show();

    playSdWav1.play("POWEROFF.WAV");
   
  function = 0;
}

void poweredOn(){
  if (playSdWav1.isPlaying() == false) 
  {
    Serial.println("Playing HUM sound");
    playSdWav1.play("HUM.WAV");
    delay(10);
  }
}

void loop()
{
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
