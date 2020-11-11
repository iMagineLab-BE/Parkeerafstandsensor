#include "FastLED.h"
#define NUM_LEDS 20
#define DATA_PIN 13
#define BRIGHTNESS 255
#define COLOR_ORDER GRB
#define LED_TYPE WS2812B

//pins used by the different wake-up sources
#define PIR_RG_PIN 12
#define POWER_STATE_PIN 32
#define SWITCH_STATE_PIN 34
#define LED_SLEEP_PIN 14

// flags to indicate LED color
#define LED_OFF 0
#define LED_GRN 1
#define LED_ORG 2
#define LED_RED 3

// Global Variables for LED-strips
CRGB leds[NUM_LEDS];

int led_offset;

void setup() {
  pinMode(SWITCH_STATE_PIN, INPUT);
  pinMode(POWER_STATE_PIN, INPUT);
  pinMode(PIR_RG_PIN, INPUT);
  pinMode(LED_SLEEP_PIN, OUTPUT);
  
  // Initialize LEDs
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);

  led_offset = 0;
}



void led_state(int ledstate) {
  int r = 0;
  int g = 0;
  int b = 0;
  switch(ledstate) {
    case LED_GRN:
      g = 255;
      break;
    case LED_ORG:
      r = 255;
      g = 140;
      break;
    case LED_RED:
      r = 255;
      digitalWrite(LED_SLEEP_PIN, LOW);
      break;
    case LED_OFF:
      digitalWrite(LED_SLEEP_PIN, HIGH);
    default:
      break;
  }
  for (int i = 0; i < NUM_LEDS; ++i) {
      if((i == led_offset) || (i == (led_offset + 1) % NUM_LEDS) || (i == (led_offset + 2) % NUM_LEDS) || (i == (led_offset + 3) % NUM_LEDS) || (i == (led_offset + 4) % NUM_LEDS) || (i == (led_offset + 5) % NUM_LEDS) || (i == (led_offset + 6) % NUM_LEDS) || (i == (led_offset + 7) % NUM_LEDS) || (i == (led_offset + 8) % NUM_LEDS) || (i == (led_offset + 9) % NUM_LEDS)) 
        leds[i] = CRGB(0,0,0);
      else
        leds[i] = CRGB(r,g,b);
  }
  FastLED.show();
  if(led_offset > NUM_LEDS - 1)
    led_offset = 0;
  else
    led_offset++;
}

void hybernate(int source) {
  if(source == PIR_RG_PIN)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIR_RG_PIN, 1);
  else if(source == POWER_STATE_PIN)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)POWER_STATE_PIN, 0);
  else
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SWITCH_STATE_PIN, 0);

  esp_deep_sleep_start();
}

void loop() {
  int pir_val = digitalRead(PIR_RG_PIN);
  int power_val = digitalRead(POWER_STATE_PIN);
  int switch_val = digitalRead(SWITCH_STATE_PIN);
  if(pir_val && (power_val == 0) && (switch_val == 0))
    led_state(LED_RED);
  else {
    led_state(LED_OFF);
    if(switch_val == 1)
      hybernate(SWITCH_STATE_PIN);
    else if(power_val == 1)
      hybernate(POWER_STATE_PIN);
    else
      hybernate(PIR_RG_PIN);
  }
  delay(100);
}
