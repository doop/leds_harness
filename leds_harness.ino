#include <FastLED.h>

#define NUM_LEDS 91
#define DATA_PIN 7
#define BUTTON_PIN 2
#define NUM_HEX_LEDS 37
CRGB leds[NUM_LEDS+NUM_HEX_LEDS]; // CRGB is effectively an array of uint8_t

struct ledmode {
  void (*initfn)(void );
  void (*updatefn)(void );
  bool reverse;
};

#define MAXBRIGHT 128

const uint8_t  sine_wave[256] = {
  0x80, 0x83, 0x86, 0x89, 0x8C, 0x90, 0x93, 0x96,
  0x99, 0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE,
  0xB1, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC1, 0xC4,
  0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8,
  0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8,
  0xEA, 0xEB, 0xED, 0xEF, 0xF0, 0xF1, 0xF3, 0xF4,
  0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC,
  0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFD,
  0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6,
  0xF5, 0xF4, 0xF3, 0xF1, 0xF0, 0xEF, 0xED, 0xEB,
  0xEA, 0xE8, 0xE6, 0xE4, 0xE2, 0xE0, 0xDE, 0xDC,
  0xDA, 0xD8, 0xD5, 0xD3, 0xD1, 0xCE, 0xCC, 0xC9,
  0xC7, 0xC4, 0xC1, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3,
  0xB1, 0xAE, 0xAB, 0xA8, 0xA5, 0xA2, 0x9F, 0x9C,
  0x99, 0x96, 0x93, 0x90, 0x8C, 0x89, 0x86, 0x83,
  0x80, 0x7D, 0x7A, 0x77, 0x74, 0x70, 0x6D, 0x6A,
  0x67, 0x64, 0x61, 0x5E, 0x5B, 0x58, 0x55, 0x52,
  0x4F, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3F, 0x3C,
  0x39, 0x37, 0x34, 0x32, 0x2F, 0x2D, 0x2B, 0x28,
  0x26, 0x24, 0x22, 0x20, 0x1E, 0x1C, 0x1A, 0x18,
  0x16, 0x15, 0x13, 0x11, 0x10, 0x0F, 0x0D, 0x0C,
  0x0B, 0x0A, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04,
  0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
  0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0A,
  0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x13, 0x15,
  0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
  0x26, 0x28, 0x2B, 0x2D, 0x2F, 0x32, 0x34, 0x37,
  0x39, 0x3C, 0x3F, 0x41, 0x44, 0x47, 0x4A, 0x4D,
  0x4F, 0x52, 0x55, 0x58, 0x5B, 0x5E, 0x61, 0x64,
  0x67, 0x6A, 0x6D, 0x70, 0x74, 0x77, 0x7A, 0x7D
};

unsigned long tap_tau = 800; //429; // 429ms ~ 140bpm
unsigned long tap_t0 = 0;

/*************************** LED Modes *****************************/


void meteor_loop() {



  float t = (millis() - tap_t0)/1000.0;
  float lambda0 = 128.0;
  float lambda1 = 200.0;
  float tau0 = tap_tau/1000.0;

  unsigned int wval;

  float fdist = t/tau0 * NUM_LEDS;

  int idist = fdist;
  idist = idist%NUM_LEDS;
  int mlen = NUM_LEDS/3;
  int head = idist + mlen;

  for (int i = 0; i < NUM_LEDS; i++) {

    unsigned int whue = wave(0,t,lambda1,60.0);
    int m = head - i;
    if ( (m<0) || (m>mlen)) {
      leds[i] = CRGB(0,0,0);
    } else if (0==m) {
      leds[i] = CRGB(255,255,255);
    } else if (m<(mlen/3)) {
      leds[i] = CHSV(whue,255,MAXBRIGHT);
    } else if (m<mlen) {
      leds[i] = CHSV(whue,255,MAXBRIGHT/2);
    } else {
      leds[i] = CRGB(0,0,0);
    }
    
  }

}


// red / blue stripes

void stripes_init() {
  CRGB c1 = CRGB(MAXBRIGHT,0,0);
  CRGB c2 = CRGB(0,0,MAXBRIGHT);
  unsigned int i=0;
  unsigned int j=0;
  int is1=0;
  CRGB c = c1;
  while (i<NUM_LEDS) {
    
    if (0==is1) {
      leds[i] = c1;
    } else {
      leds[i] = c2;
    }
    i++;
    j=(j+1)%8;
    if (0==j) {
      is1 = 1-is1;
    }
  }
  roll_init();
}


// loop fn to take the existing pattern and roll around the LED strip
unsigned long roll_last;

void roll_init() {
  roll_last = millis();
}

void do_roll() {
  CRGB first = leds[0];
  for (unsigned int i=0;i<(NUM_LEDS-1);i++) {
    leds[i] = leds[i+1];
  }
  leds[NUM_LEDS-1] = first;
}

void roll_pattern() {
  unsigned long t = millis()-tap_t0;
  if ( (t-roll_last) > tap_tau ) {
    roll_last = t;
    do_roll();
  }
}

// Make everything red

void testred_loop() {

      all_color(MAXBRIGHT,0,0);
}

// Flash blue/green

void testflash_loop() {
    unsigned int flashms = 500;
    unsigned int t = millis()-tap_t0;
    if (0==((t/tap_tau)%2)) {
      all_color(0,MAXBRIGHT,0);
    } else {
      all_color(0,0,MAXBRIGHT);
    }
}

// Rainbow



void rainbow_loop() {
  float t = (millis()-tap_t0)/1000.0;
  float lambda0 = 128.0;
  float tau0 = tap_tau/1000.0 * 4;

  for (int i = 0; i < NUM_LEDS; i++) {

    unsigned int whue = wave(i,t,lambda0,tau0);
    
    leds[i] = CHSV(whue, 255, MAXBRIGHT);
  }
}

// Rainbow pulsing in intensity

void rwave_loop() {
  float t = (millis()-tap_t0)/1000.0;
  float lambda0 = 128.0;
  float tau0 = tap_tau/1000.0;
  float lambda1 = 200.0;
  float tau1 = 900.0;
  for (int i = 0; i < NUM_LEDS; i++) {

    unsigned int wval = wave(i,t,lambda0,tau0) >> 1;
    unsigned int whue = wave(i,t,lambda1,tau1);
    
    leds[i] = CHSV(whue, 255, wval);
  }
}

void dark_loop() {

  for (int i = 0; i < NUM_LEDS; i++) {

    leds[NUM_LEDS-1-i] = CRGB(0,0,0);
  }

}

void lines_loop() {
  float t = (millis() - tap_t0)/1000.0;
  float lambda_line = 128.0;
  float tau_line = tap_tau/1000.0;
  float lambdal1 = 200.0;

  for (int i = 0; i < NUM_LEDS; i++) {
    unsigned int wval;
    unsigned int thewave = wave(i,t,lambda_line,tau_line);
    if (thewave > 200) { wval = MAXBRIGHT; } else { wval = 0; }
    
    unsigned int whue = wave(0,t,lambdal1,60.0);
    unsigned int wsat = 255;
    // if (thewave > 250) { wsat = 0; }

    leds[NUM_LEDS-1-i] = CHSV(whue, wsat, wval);
  }

}

// Pulsing rainbows going outwards
void radiate_loop() {
  float t = (millis() - tap_t0)/1000.0;
  float lambda0 = 50.0;
  float lambda1 = 200.0;
  float tau0 = tap_tau/1000.0;

  for (int i = 0; i < NUM_LEDS; i++) {

    unsigned int wval = wave(i,t,lambda0,tau0) >> 1;
    unsigned int whue = wave(0,t,lambda1,60.0);

    leds[i] = CHSV(whue, 255, wval);
  }

}

void smoothlines_loop() {



  float t = (millis() - tap_t0)/1000.0;
  float lambda0 = 128.0;
  float lambda1 = 200.0;
  float tau0 = tap_tau/1000.0;

  for (int i = 0; i < NUM_LEDS; i++) {

    unsigned int wval = ( wave(i,t,lambda0,tau0) );
    unsigned int whue = wave(0,t,lambda1,60.0);
    if (wval<200) { wval = 0; } else { wval = dim8_raw((wval*2)/3); }
    //if (wval<64) { wval = 0; } else { wval /= 2; }

    leds[i] = CHSV(whue, 255, wval);
  }

}

void pulses_loop() {
  float t = (millis() - tap_t0)/1000.0;
  float lambda0 = 32.0;
  float lambda1 = 200.0;
  float tau0 = tap_tau/1000.0;

  for (int i = 0; i < NUM_LEDS; i++) {

    unsigned int thewave = wave(i,t,lambda0,tau0) ;
    unsigned int whue = wave(0,t,lambda1,60.0);
    unsigned int wval = MAXBRIGHT>>1;
    int wave_on = 0;

    if (thewave >128) { wave_on = 1; }

    if (wave_on) {
      wval = MAXBRIGHT/2;
    } else {
      
      whue = (whue+85)%256;
    }

    leds[i] = CHSV(whue, 255, wval);

  }

}

void xpulses_loop() {
  float t = millis()/1000.0;
  const float plambda0 = 90.0;
  const float ptau0 = 1.0;
  const float plambda1 = 200.0;
  const float ptau1 = 900.0;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    unsigned int wval = 64;
    unsigned int thewave = wave(i,t,plambda0,ptau0);
    unsigned int whue = wave(0,t,plambda1,60.0);
    int wave_on = false;
    
    if ((thewave <16) || (thewave > 250)) { wave_on = 1; }
    if (wave_on) {
      wval = 128;
    } else {
      whue = 256-whue;
    }
    
    unsigned int wsat = MAXBRIGHT;
    // if (thewave > 250) { wsat = 0; }
    leds[i] = CHSV(whue, MAXBRIGHT, wval);

  }
  
}


struct ledmode modes[] = {
  // { NULL, dark_loop, false } , // uncomment to make the initial state dark


 { NULL, rainbow_loop, true },

 { NULL, smoothlines_loop, true },
 { NULL, lines_loop, false },
 { NULL, radiate_loop, true },


  { NULL, pulses_loop, true },
  { NULL, testflash_loop, false},
 
  { NULL, NULL, false },
};

/***** LED utility functions ******/

void all_color(uint8_t r, uint8_t g, uint8_t b) {
 
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(r,g,b);
  }
}

unsigned int wave(int i, float t, float lambda, float tau) {
    unsigned int phase = ((unsigned int)(256*( ((float)i)/lambda - t/tau))) & 0xff;
    unsigned int waveval = sine_wave[phase];
    return waveval;
}

void reverse_leds() {
    CRGB tmp;

    for (int i=0;i<NUM_LEDS/2;i++) {
        tmp = leds[i];
        leds[i] = leds[NUM_LEDS-i-1];
        leds[NUM_LEDS-i-1]=tmp; 
    }
}


const unsigned char hex_0[]  = {18, 0xff};
const unsigned char hex_1[] = { 11,12,  17, 19,  24, 25, 0xff};
const unsigned char hex_2[] = { 5, 6, 7,  10,  13, 16, 20, 23, 26, 29, 30, 31, 0xff };
const unsigned char hex_3[] = { 0, 1, 2, 3, 4, 8, 9, 14, 15, 21, 22, 27, 28, 32, 33, 34, 35, 36, 0xff};
const unsigned char *hexlayers[]= { hex_0, hex_1, hex_2, hex_3 };

// NB the segments overlap!
const unsigned char hexseg_0[] = {  0,  1,  2,  3,  5,  6,  7, 11, 12, 18, 0xff };
const unsigned char hexseg_1[] = {  3,  8, 14, 21,  7, 13, 20, 12, 19, 18, 0xff };
const unsigned char hexseg_2[] = { 21, 27, 32, 36, 20, 26, 31, 19, 25, 18, 0xff };
const unsigned char hexseg_3[] = { 36, 35, 34, 33, 31, 30, 29, 25, 24, 18, 0xff };
const unsigned char hexseg_4[] = { 33, 28, 22, 15, 29, 23, 16, 24, 17, 18, 0xff };
const unsigned char hexseg_5[] = { 15,  9,  4,  0, 16, 10,  5, 17, 11, 18, 0xff };
const unsigned char *hexsegs[] = { hexseg_0, hexseg_1, hexseg_2, hexseg_3, hexseg_4, hexseg_5 };
const unsigned char hexseg_centre[] = { 6, 13, 26, 30, 23, 10 };

const unsigned char hexseg_level[] = {
  3, 3, 3, 3, 3, 2, 2, 2,
  3, 3, 2, 1, 1, 2, 3, 3,
  2, 1, 0, 1, 2, 3, 3, 2,
  1, 1, 2, 3, 3, 2, 2, 2,
  3, 3, 3, 3, 3, 3
}; // Map LED number to distance from centre

void hexlist(const unsigned char *indices, CRGB c) {
  // Take a pointer to an array of indices terminated by 0xff
  // Switch on all hex LEDs whose (0-based) indices are in the array
  unsigned int i=0;
  unsigned char ledno=indices[0];
  while (ledno != 0xff)  {
    leds[NUM_LEDS+ledno] = c;
    ledno = indices[i++];
  }    
}

CRGB cb_null(void *cbstatic) {
  return *(CRGB *)cbstatic;
}

void hexlist_iter(const unsigned char *indices, CRGB (*cb)(unsigned char , void *) , void *cbstatic) {
  // Take a pointer to an array of indices terminated by 0xff
  // Invoke callback for all hex LEDs whose (0-based) indices are in the array
  unsigned int i=0;
  unsigned char ledno=indices[0];
  while (ledno != 0xff)  {
    CRGB c = cb(ledno, cbstatic);
    leds[NUM_LEDS+ledno] = c;
    ledno = indices[i++];
  }   
}

void hexseg_iter(unsigned int n, CRGB (*cb)(unsigned char , void *) , void *cbstatic) {
  if (n>5) { n=5; }
  return hexlist_iter(hexsegs[n],cb,cbstatic);
}

void hexring_iter(unsigned int ringno, CRGB (*cb)(unsigned char , void *) , void *cbstatic) {
  if (ringno>3) { ringno=3; }
  return hexlist_iter(hexlayers[ringno],cb,cbstatic);
}

void hexfill_iter(CRGB (*cb)(unsigned char , void *) , void *cbstatic) {
  for (unsigned int i=0;i<4;i++) {
    hexring_iter(i,cb,cbstatic);
  }
}

void hexring(unsigned int ringno, CRGB c) {
  if (ringno>3) { ringno=3; }
  return hexlist(hexlayers[ringno],c);
  
}
void hexrings(CRGB c0, CRGB c1, CRGB c2, CRGB c3) {
    hexring(0,c0);
    hexring(1,c1);
    hexring(2,c2);
    hexring(3,c3);
}
void hexseg(unsigned int n, CRGB c) {
  if (n>5) { n=5; }
  return hexlist(hexsegs[n],c);
}



void hexfill(CRGB c) {
  hexring(0,c);
  hexring(1,c);
  hexring(2,c);
  hexring(3,c);
}

void hextri_a_iter(CRGB (*cb)(unsigned char , void *) , void *cbstatic) {
    hexseg_iter(0,cb,cbstatic);
    hexseg_iter(2,cb,cbstatic);
    hexseg_iter(4,cb,cbstatic);
}

void hextri_b_iter(CRGB (*cb)(unsigned char , void *) , void *cbstatic) {
    hexseg_iter(1,cb,cbstatic);
    hexseg_iter(3,cb,cbstatic);
    hexseg_iter(5,cb,cbstatic);
}

void do_hex() {

  // Render the central hexagon, if present

#if (0 == NUM_HEX_LEDS)
  // do nothing
#else
  hexring(0,leds[NUM_LEDS-1]);
  hexring(1,leds[NUM_LEDS-1-4]);
  hexring(2,leds[NUM_LEDS-1-8]);
  hexring(3,leds[NUM_LEDS-1-12]);
#endif
}

/************************* Button handling *************************/

int button_state;
int last_button_state = LOW;
unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 10; // ms
unsigned long last_change_time = 0;
unsigned long last_on_time = 0;

unsigned int modeno = 0;
const unsigned long long_press_ms = 1000; // holding two seconds counts as a long press
unsigned int sync_mode = false;

void long_press() {
  sync_mode = true;
}


void next_mode() {
    modeno++;
     
    struct ledmode *thismode = &modes[modeno];
    if ( (NULL==thismode->initfn) && (NULL==thismode->updatefn) ) {
      modeno = 0;
      thismode = &modes[modeno];
    }
    update_mode();
}

void prev_mode() {
  struct ledmode *thismode = &modes[modeno];

  if (0==modeno) {
      modeno = sizeof(modes)/sizeof(struct ledmode) -2;
  } else {
    modeno--;
  }
  update_mode();
}

void update_mode() {
    struct ledmode *thismode = &modes[modeno];
    if (NULL!=thismode->initfn) {
      thismode->initfn();
    } 
}

void button_pressed() {
  next_mode();
}

#define MAXTAPS 8
unsigned long taps[MAXTAPS];

size_t ntaps = 0;

void sync_taps() {
  if (0==ntaps) { return; }
  if (1==ntaps) { tap_t0 = taps[0]; }

  float st = 0.0;
  float snt = 0.0;
  for (int i=0;i<ntaps;i++) {
    st = st + (float)taps[i];
    snt = snt + ((float)i)*(float)taps[i];
  }
  float n=(float)ntaps;
 
  st = st/n;
  snt = snt/n;
  float A = 0.5*(n-1.0);
  float B = (n-1.0)*(2.0*n-1.0)/6.0;
  float C = B-A*A;

  float t0 = (B*st - A*snt)/C;
  float tau = (snt - A*st)/C;
  tap_t0 = (unsigned long) t0;
  tap_tau = (unsigned long) tau;
}

void button_press_syncmode() {
    unsigned long t = last_debounce_time;
    taps[ntaps++] = t;
    sync_taps();
    if (ntaps>=MAXTAPS) {
      ntaps = 0;
      sync_mode = false;
    }
}

void button_pressed_outer() {
    if (sync_mode) {
        button_press_syncmode();
    } else {
        button_pressed();
    }
}

int in_long_press = false;

void process_button() {

  int reading = digitalRead(BUTTON_PIN);
  unsigned long now = millis();
  if (reading != last_button_state) {
    last_debounce_time = now;
  }

  // Check for long press
  if ( (!in_long_press) && (HIGH == reading) && ( (now - last_debounce_time) > long_press_ms) ) {
    in_long_press = true;
    if (!sync_mode) {
        prev_mode(); // undo next-mode
    }
    sync_mode = true;

  } else {
    in_long_press = false;
  }

  if ((now - last_debounce_time) > debounce_delay) {
      if (reading != button_state) {
          // button state has changed
          button_state = reading;
          if (HIGH == button_state) {
            button_pressed_outer();
          }
          last_change_time = last_debounce_time;
      }
  }
  last_button_state = reading;
}


/*************** Core *******************/

void setup() {
  pinMode(BUTTON_PIN,INPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS + NUM_HEX_LEDS);
  //Serial.begin(9600);
}

CRGB cb_darker_outside(unsigned char ledno, void  *cbstatic) {
  int nshift[] = { 0, 2, 3, 4, 5};
  CRGB c = *(CRGB *)cbstatic;
  unsigned char hlevel = hexseg_level[ledno];
  CRGB cc = c;

  cc.r >>=nshift[hlevel];
  cc.g >>=nshift[hlevel];
  cc.b >>=nshift[hlevel];
  return cc;
}

CRGB cb_darker_inside(unsigned char ledno, void  *cbstatic) {
  int nshift[] = { 4, 3, 2, 0 };
  CRGB c = *(CRGB *)cbstatic;
  unsigned char hlevel = hexseg_level[ledno];
  CRGB cc = c;

  cc.r >>=nshift[hlevel];
  cc.g >>=nshift[hlevel];
  cc.b >>=nshift[hlevel];
  return cc;
}

void draw_sync_mode() {
  unsigned long t = (millis() - tap_t0);
  bool intap = false;
  if ( 0==(t/tap_tau)%2 ) {
      intap = true;
  }

  CRGB bgcolor = CRGB(MAXBRIGHT>>1,0,0);
  CRGB fgcolor = CRGB(0,MAXBRIGHT>>1,0);
  if (intap) {
    bgcolor = CRGB(0,0,MAXBRIGHT>>1);
  }
 
  hexfill_iter(cb_darker_inside,&bgcolor);

  if ((0==ntaps)||(7==ntaps)) {
    if (intap) {
      hextri_a_iter(cb_darker_outside,&fgcolor);
    } else {
      hextri_b_iter(cb_darker_outside,&fgcolor);
    }
  } else {
    hexseg_iter(ntaps-1,cb_darker_outside,&fgcolor);
  }
 
   
}

void loop() {
  process_button();
  void (*updatefn)(void ) = modes[modeno].updatefn;
  if (NULL != updatefn) {
    modes[modeno].updatefn();
  }
  if (modes[modeno].reverse) {
      reverse_leds();
  }
  do_hex();
  if (sync_mode) {
    draw_sync_mode();
  }
  FastLED.show();
  //Serial.write('*');
}
