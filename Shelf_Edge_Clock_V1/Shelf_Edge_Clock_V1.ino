/*
 * 3D printed smart shelving with a giant hidden digital clock in the front edges of the shelves - DIY Machines

==========

More info and build instructions: https://www.youtube.com/watch?v=8E0SeycTzHw

3D printed parts can be downloaded from here: https://www.thingiverse.com/thing:4207524

You will need to install the Adafruit Neopixel library which can be found in the library manager.

This project also uses the handy DS3231 Simple library:- https://github.com/sleemanj/DS3231_Simple   Please follow the instruction on installing this provided on the libraries page

Before you install this code you need to set the time on your DS3231. Once you have connected it as shown in this project and have installed the DS3231_Simple library (see above) you
 to go to  'File' >> 'Examples' >> 'DS3231_Simple' >> 'Z1_TimeAndDate' >> 'SetDateTime' and follow the instructions in the example to set the date and time on your RTC

==========


 * SAY THANKS:

Buy me a coffee to say thanks: https://ko-fi.com/diymachines
Support us on Patreon: https://www.patreon.com/diymachines

SUBSCRIBE:
■ https://www.youtube.com/channel/UC3jc4X-kEq-dEDYhQ8QoYnQ?sub_confirmation=1

INSTAGRAM: https://www.instagram.com/diy_machines/?hl=en
FACEBOOK: https://www.facebook.com/diymachines/

*/
#define DEBUG false

#include <Adafruit_NeoPixel.h>

#include <DS3231_Simple.h>
DS3231_Simple Clock;

// Create a variable to hold the time data
DateTime MyDateAndTime;

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDCLOCK_PIN    6
#define LEDDOWNLIGHT_PIN    5

// How many NeoPixels are attached to the Arduino?
#define LEDCLOCK_COUNT 216
#define LEDDOWNLIGHT_COUNT 12

//(red * 65536) + (green * 256) + blue ->for 32-bit merged colour value so 16777215 equals white
long clockMinuteColour = 51200; //1677
long clockHourColour = 140000000; //7712

int clockFaceBrightness = 0;

// Declare our NeoPixel objects:
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

#include "smoothiner.hpp"

Smoothiner smoothiner;

void setup() {

#if DEBUG
  Serial.begin(9600);
#endif //DEBUG

  Clock.begin();

  stripClock.begin();           // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.show();            // Turn OFF all pixels ASAP
  stripClock.setBrightness(100);

  stripDownlighter.begin();
  stripDownlighter.show();
  stripDownlighter.setBrightness(50);

  smoothiner.setup(analogRead(A0));
}

void loop() {

  readTheTime();
  displayTheTime();

  // Record a reading from the light sensor and add it to the smoothiner
  smoothiner.addReading(analogRead(A0));

  //set the brightness based on ambiant light levels
  long lightSensorValue = smoothiner.getAverage();
  clockFaceBrightness = map(lightSensorValue, 50, 1000, 200, 1);
  stripClock.setBrightness(clockFaceBrightness); // Set brightness value of the LEDs

#if DEBUG
  Serial.print("Mapped brightness value = ");
  Serial.println(clockFaceBrightness);
#endif //DEBUG

  stripClock.show();

   //(red * 65536) + (green * 256) + blue ->for 32-bit merged colour value so 16777215 equals white
  stripDownlighter.fill(16777215, 0, LEDDOWNLIGHT_COUNT);
  stripDownlighter.show();

  delay(5000);   //this 5 second delay to slow things down during testing
}


void readTheTime() {

  MyDateAndTime = Clock.read();

#if DEBUG
  Serial.println("");
  Serial.print("Time is: ");   Serial.print(MyDateAndTime.Hour);
  Serial.print(":"); Serial.print(MyDateAndTime.Minute);
  Serial.print(":"); Serial.println(MyDateAndTime.Second);
  Serial.print("Date is: 20");   Serial.print(MyDateAndTime.Year);
  Serial.print(":");  Serial.print(MyDateAndTime.Month);
  Serial.print(":");    Serial.println(MyDateAndTime.Day);
#endif //DEBUG
}

void displayTheTime() {

  stripClock.clear();

  // work out the value of the first digit and then display it
  int firstMinuteDigit = MyDateAndTime.Minute % 10;
  displayNumber(firstMinuteDigit, 0, clockMinuteColour);

  // work out the value for the second digit and then display it
  int secondMinuteDigit = floor(MyDateAndTime.Minute / 10);
  displayNumber(secondMinuteDigit, 63, clockMinuteColour);

  // work out the value for the third digit and then display it
  int firstHourDigit = MyDateAndTime.Hour;
  if (firstHourDigit > 12) {
    firstHourDigit -= 12;
  }

  firstHourDigit %= 10;
  displayNumber(firstHourDigit, 126, clockHourColour);

  // work out the value for the fourth digit and then display it
  int secondHourDigit = MyDateAndTime.Hour;
  if (secondHourDigit > 12) {

    secondHourDigit -= 12;
  }

  if (secondHourDigit > 9) {

    stripClock.fill(clockHourColour, 189, 18);
  }
}
