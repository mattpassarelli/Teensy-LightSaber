#include <OctoWS2811.h>

const int ledsPerStrip = 120;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB || WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  // put your setup code here, to run once:
  leds.begin();
  leds.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("LEDS ON");
  for(int i = 0; i < ledsPerStrip; i++){
    leds.setPixel(i, 255,255,255);
  }

  leds.show();
}