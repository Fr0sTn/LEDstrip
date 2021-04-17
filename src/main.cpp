/////////////////////////////////////////////
// DEFINING THE LED STRIP

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define DATA_PIN            6
#define NUM_LEDS            240
#define MAX_POWER_MILLIAMPS 40
#define LED_TYPE            WS2812
#define COLOR_ORDER         GRB

CRGB leds[NUM_LEDS];

/////////////////////////////////////////////
// DEFINING THE PUSH BUTTON & BUZZER

const int buttonPin = 2;
int buttonState;
const int buzzerPin = 8;


/////////////////////////////////////////////
//DEFINING THE SOUND SENSOR

const int soundPin = 4;
int sound;

/////////////////////////////////////////////
// OTHER KINDA IMPORTANT STUFF

unsigned long startMillis = 0;
unsigned long currentMillis = 0;
const unsigned long period = 150; // = 0.15 second (that's quick)
int mode = 0;
unsigned long startWaveMillis = 0;
  unsigned long curWaveMillis = 0;
int randLed;

void mode_0();
void mode_2();
void mode_3();
void checkSound();
void shift();
void pacifica_loop();
void pacifica_add_whitecaps();
void pacifica_deepen_colors();
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff);

/////////////////////////////////////////////


void setup() 
{
/////////////////////////////////////////////
//INITIAL STARTUP

  Serial.begin(115200);
  delay(1000); // a 1 sec delay to give the controller time to wake up peacefully
  startMillis = millis();
  startWaveMillis = millis();
  
/////////////////////////////////////////////
//LED STRIP INITIAL SETUP

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
        .setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
  randLed = 0;

/////////////////////////////////////////////
//PUSH BUTTON & BUTTON SETUP

  pinMode(buttonPin, INPUT_PULLUP);
  buttonState = 0;
  pinMode(buzzerPin,OUTPUT);

/////////////////////////////////////////////
//MICROPHONE SETUP

pinMode(soundPin,INPUT_PULLUP);

}

/////////////////////////////////////////////
//END OF SETUP


void loop()
{
/////////////////////////////////////////////
//MAIN LOOP (TO BE USED MAINLY AS A 'SWITCH' FUNCTION)

currentMillis = millis();
if (currentMillis - startMillis >= period)  //wait for the desired period
{
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == LOW)
    {
      mode += 1;
      tone (buzzerPin, 1300, 1000);
      delay(200);
      noTone(buzzerPin);
    }
    startMillis = currentMillis;
}
  switch (mode)
  {
    case 0:
      mode_0();
      break;
      
    case 1:
      EVERY_N_MILLISECONDS(15) // updates each 15 milliseconds (this directly affects how smooth the waves are) 
      {
        pacifica_loop();
        FastLED.show();
      }
      break;
      
    case 2:
      mode_2 ();
      break;

    case 3:
      mode_3 ();
      break;
      
    case 4:
      mode = 0;
    break;
   }
    
}


/////////////////////////////////////////////
// MODE 0

void mode_0 () 
{
unsigned long delayMillis = 0;
const unsigned long delayPeriod = 500;
int starLed;
fill_solid ( leds, NUM_LEDS, CRGB::Purple);
FastLED.show();

  long randomStar = random(1, 500000);

  if (currentMillis - delayMillis >= delayPeriod)
 {
   if (randomStar <= 2)
   {
   for(starLed = 0; starLed < (NUM_LEDS+1); starLed += 1) 
      {
      leds[starLed] = CRGB::White;
      FastLED.show(); // Update the leds (two of which is set to another colour, from above)
      delay(15);      // Wait a little bit
      
      if (starLed >= 1)
        {
         leds[(starLed-1)] = CRGB::Purple;
        }
      }
      
      leds[(starLed+1)] = CRGB::Purple;
      FastLED.show();
   }
   delayMillis = currentMillis;
 }
}

/////////////////////////////////////////////
//END OF MODE 0



/////////////////////////////////////////////
//MODE 1 (MOSTLY COPY-PASTA'D FROM OPEN SOURCES)

CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };


void pacifica_loop()
{

  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  pacifica_add_whitecaps();

  pacifica_deepen_colors();
}

void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}

/////////////////////////////////////////////
//END OF MODE 1



/////////////////////////////////////////////
//MODE 2 (BUT ACTUALLY 3)

void mode_2 ()
{
    if (randLed <= NUM_LEDS)
      {
      leds[randLed] = CHSV(random8(),255,255);
      FastLED.show(); // Update the leds
      randLed++;
      delay(100);
      }
      else
      {
      randLed = 0;
      delay(50);
      }
}
   
/////////////////////////////////////////////
//END OF MODE 2 (THE ONE THAT'S ACTUALLY MODE 3)


/////////////////////////////////////////////
//MODE 3 (USING MICROPHONE TO GET A VISUALIZER-LIKE EFFECT)

void mode_3 ()
{
  const int wavePeriod = 10;
  curWaveMillis = millis();
  if (curWaveMillis - startWaveMillis >= wavePeriod)
 {
  checkSound();
  shift();
  startWaveMillis = curWaveMillis;
 }

}

void checkSound()
{

  sound = digitalRead(soundPin);
  if(sound != 0)
   {

      leds[0] = CHSV(random8(),255,255);
      FastLED.show();
      Serial.print(sound);      
   }
   else
   {
      leds[0] = CRGB::Red;
      FastLED.show();
      Serial.print (sound);
   }
}

void shift()
{
  for (int i = NUM_LEDS; i >= 0; i--)
  {
    leds[i] = leds[i-1];
  }
  FastLED.show();
}

/////////////////////////////////////////////
//END OF MODE 3