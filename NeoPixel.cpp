#include "Adafruit_NeoPixel.h"

#define NEOPIXEL_MINUTE_PIN 2
#define NEOPIXEL_HOUR_PIN 3
#define AM_PM_PIN1 4
#define AM_PM_PIN0 5

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel stripMinute = Adafruit_NeoPixel(24, NEOPIXEL_MINUTE_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripHour   = Adafruit_NeoPixel(12, NEOPIXEL_HOUR_PIN,   NEO_GRB + NEO_KHZ800);

String ssid     = "Simulator Wifi";  // SSID to connect to
String password = ""; // Our virtual wifi has no password (so dont do your banking stuff on this network)
String host     = "nist.time.gov"; // Open Weather Map API
const int httpPort   = 80;
String uri     = "/actualtime.cgi?lzbc=1462431600000000";

int second = 60;
int minute = 55;
int hour = 15;

int setupESP8266(void) {
  // Start our ESP8266 Serial Communication
  Serial.begin(115200);   // Serial connection over USB to computer
  Serial.println("AT");   // Serial connection on Tx / Rx port to ESP8266
  delay(10);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 1;
    
  // Connect to 123D Circuits Simulator Wifi
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(10);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 2;
  
  // Open TCP connection to the host:
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 3;
  
  return 0;
}

 long getTimeESP8266(void) {
  // Construct our HTTP call
  String httpPacket = "GET " + uri + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  int length = httpPacket.length();
  
  // Send our message length
  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(10); // Wait a little for the ESP to respond
  if (!Serial.find(">")) return -1;

  // Send our http request
  Serial.print(httpPacket);
  delay(10); // Wait a little for the ESP to respond
  if (!Serial.find("SEND OK\r\n")) return -2;
  
  while(!Serial.available()) delay(5);  // wait until we receive the response from the server

  String outputString = "";  
  
  if (Serial.find("\r\n\r\n")) {  // search for a blank line which defines the end of the http header
    delay(5);
    
    unsigned int i = 0; //timeout counter
    
    while (!Serial.find("delay=\"")){ delay(1); } // find the part we are interested in.
    
    while (i<60000) { // 1 minute timeout checker
      if(Serial.available()) {
        char c = Serial.read();
        if(c=='"') break; // break out of our loop because we got all we need
        outputString += c; // append to our output string
        i=0; // reset our timeout counter
      }
      i++;
      delay(1);
    }
  }
  outputString.remove(outputString.length() - 6);
  return outputString.toInt();
}

int sychronizeTime(void) {
  drawHourHand(-1);
  long now = getTimeESP8266();
  if (now >= 0) {
    second = now % 60;
    now /= 60;
    minute = now % 60;
    now /= 60;
    hour = now % 24;
  }
}

void setPixelColor(Adafruit_NeoPixel& strip, int index, unsigned char red, unsigned char green, unsigned char blue, int brightness = 256) {
  strip.setPixelColor(
    index,
    (((int)red) * brightness) >> 8,
    (((int)green) * brightness) >> 8,
    (((int)blue) * brightness) >> 8
  );
}

#define BRIGHTNESS 256

void drawMinuteHand(void) {
  stripMinute.clear();
  int value = minute * 2;
  int index;
  int i;
  for (i = 0, index = 0; i < 120; i += 5, ++index) {
    int dist = (value > i) ? value - i : i - value;
    if (dist > 60) dist = 120 - dist;
    setPixelColor(stripMinute, index, dist < 10 ? 2 * (10 - dist) * (10 - dist) : 0, 0, 0, BRIGHTNESS);
  }
  stripMinute.show();
}

void drawHourHand(int sweep) {
  stripHour.clear();
  int hour12 = (hour > 11) ? hour - 12 : hour;
  if (sweep < 0) {
    for (int i = 0; i < 12; ++i) {
      // Display blue to show internet access
      setPixelColor(stripHour, i, 0, 64, 128, BRIGHTNESS);       
    }
  } else {
    if (hour12 == sweep) {
      setPixelColor(stripHour, sweep, 0, 128, 255, BRIGHTNESS); 
      setPixelColor(stripHour, sweep + 1, 0, 64, 128, BRIGHTNESS); 
    } else if (hour12 == sweep + 1) {
      setPixelColor(stripHour, sweep, 0, 32, 64, BRIGHTNESS); 
      setPixelColor(stripHour, sweep + 1, 0, 128, 255, BRIGHTNESS); 
    } else {
      setPixelColor(stripHour, sweep, 0, 32, 64, BRIGHTNESS); 
      setPixelColor(stripHour, sweep + 1, 0, 64, 128, BRIGHTNESS);
      setPixelColor(stripHour, hour12, 0, 128, 255, BRIGHTNESS); 
    }
  }
  stripHour.show();
}

void updateClock(void) {
  // This gets called 24 times per second.
  static int count = 0;
  
  if (++count == 24) {
    count = 0;
    // Update the time display
    if (++second >= 60) {
      // Update the minute
      if (++minute >= 60) {
        minute = 0;
        if (++hour >= 24) {
          hour = 0;
        }
      }
      second = 0;
      
      // Draw the minute hand
      drawMinuteHand();
    }
  }

  if (!(count & 3)) {
    drawHourHand(count >> 1);
    // Update AM/PM
    digitalWrite(AM_PM_PIN0, LOW);
    digitalWrite(AM_PM_PIN1, (hour >= 12) ? HIGH : LOW);
  }
}

ISR(TIMER2_COMPA_vect) {
  static unsigned char count = 0;
  // This interrupt is being called 125 times per second.
  // Trigger a function call exactly 24 times per second using math:
  count += 24;
  if (count >= 125) {
    count -= 125;
    updateClock();
  }
}

void setup() {
  digitalWrite(AM_PM_PIN0, HIGH);
  pinMode(AM_PM_PIN0, OUTPUT);
  digitalWrite(AM_PM_PIN1, HIGH);
  pinMode(AM_PM_PIN1, OUTPUT);
  
  stripMinute.begin();
  stripHour.begin();

  stripMinute.show(); // Initialize all pixels to 'off'
  stripHour.show(); // Initialize all pixels to 'off'
  
  setupESP8266();
  sychronizeTime();
  drawMinuteHand();

    // Set up timer2 to interrupt 125 times per second
  TCCR2B = 0x00;        //Disbale Timer2 while we set it up
  TCNT2  = 0;           //Reset Timer Count to 0 out of 255
  OCR2A  = 125;         //Counter compare value
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK2 = 0x02;        //Timer2 INT Reg: Timer2 compare A interrupt
  TCCR2A = 0x02;        //Timer2 Control Reg A: CTC
  TCCR2B = 0x07;        //Timer2 Control Reg B: Timer Prescaler set to 1024
}

void loop() {
  static int lastSecond = 0;
  if (second == 45 && lastSecond == 44) {
    // Once per minute (at 45 seconds after the minute), sychronize the time
    sychronizeTime();
  }
  lastSecond = second;
  delay(100);
}