/*
 *
 *   Copyright (C) 2021 Erich S. Heinzle
 *   Website https://github.com/Lobethal-Lutheran-School/
 *   
 *   Lobethal Lutheran School - "Historically Grounded, Future Minded"
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License at <http://www.gnu.org/licenses/> 
 *   for more details.
 *
 */

/*
 * TODO:
 *        implement audio DSP on audio input
 *        
 */

#define DEBUG 0

// Christmas Light Controller Pin Allocations (Digital Pins)
#define CHAN_1_INH PA0
#define CHAN_2_INH PA1
#define CHAN_3_INH PA2
#define CHAN_4_INH PA3
#define CHAN_5_INH PB6
#define CHAN_6_INH PB7
#define CHAN_7_INH PB8
#define CHAN_8_INH PB9

#define CHAN_1_IN1 PA4
#define CHAN_1_IN2 PA5

#define CHAN_2_IN1 PA6
#define CHAN_2_IN2 PA7

#define CHAN_3_IN1 PA8
#define CHAN_3_IN2 PA9

#define CHAN_4_IN1 PA10
#define CHAN_4_IN2 PA11

#define CHAN_5_IN1 PB0
#define CHAN_5_IN2 PB1

#define CHAN_6_IN1 PB3
#define CHAN_6_IN2 PB4

#define CHAN_7_IN1 PB12
#define CHAN_7_IN2 PB13

#define CHAN_8_IN1 PB14
#define CHAN_8_IN2 PB15

#define CHANNEL1 0x00000001
#define CHANNEL2 0x00000010
#define CHANNEL3 0x00000100
#define CHANNEL4 0x00001000
#define CHANNEL5 0x00010000
#define CHANNEL6 0x00100000
#define CHANNEL7 0x01000000
#define CHANNEL8 0x10000000

#define CHANNEL_BUTTON PB11
#define MODE_BUTTON PB10

#define AUDIO_CHAN1 PA12
#define AUDIO_CHAN2 PA15

#define LED PC13

// MORSE timings in ms
#define DIT_DURATION_MS 300
#define DAH_DURATION_MS 900
#define LETTER_SPACE 900
#define WORD_SPACE 2100

#define MORSE_STRING "LOBETHAL LUTHERAN SCHOOL  "

enum ledmode{ALL_ON,
              SLOW_FLASH,
              SLOW_FLASH_RAMPED,
              FAST_FLASH,
              RANDOM,
              PONG,
              CHASE,
              PONG_RAMP,
              CHASE_RAMP,
              MORSE_TEXT,
              ALTERNANS};

typedef struct context_t {
  int channels;
  int selected_channel; // doesn't do much yet
  ledmode mode;
  int unidirectional_duration_ms;
};

typedef struct morse_t {
  int morse[6];
};

static const morse_t morse_lookup[36] =
                     {{1,2,0,0,0,0}, // a
                      {2,1,1,1,0,0}, // b
                      {2,1,2,1,0,0}, // c
                      {2,1,1,0,0,0}, // d
                      {1,0,0,0,0,0}, // e
                      {1,1,2,1,0,0}, // f
                      {2,2,1,0,0,0}, // g
                      {1,1,1,1,0,0}, // h
                      {1,1,0,0,0,0}, // i
                      {1,2,2,2,0,0}, // j
                      {2,1,2,0,0,0}, // k
                      {1,2,1,1,0,0}, // l
                      {2,2,0,0,0,0}, // m
                      {2,1,0,0,0,0}, // n
                      {1,1,1,0,0,0}, // o
                      {1,2,2,1,0,0}, // p
                      {2,2,1,2,0,0}, // q
                      {1,2,1,0,0,0}, // r
                      {1,1,1,0,0,0}, // s
                      {2,0,0,0,0,0}, // t
                      {1,1,2,0,0,0}, // u
                      {1,1,2,0,0,0}, // v
                      {1,1,1,2,0,0}, // w
                      {1,2,2,0,0,0}, // x
                      {2,1,2,2,0,0}, // y
                      {2,2,1,1,0,0}, // z
                      {2,2,2,2,2,0}, // 0
                      {1,2,2,2,2,0}, // 1
                      {1,1,2,2,2,0}, // 2
                      {1,1,1,2,2,0}, // 3
                      {1,1,1,1,2,0}, // 4
                      {1,1,1,1,1,0}, // 5
                      {2,1,1,1,1,0}, // 6
                      {2,2,1,1,1,0}, // 7
                      {2,2,2,1,1,0}, // 8
                      {2,2,2,2,1,0}};// 9

                      
void setup() {
  pinMode(CHAN_1_INH, OUTPUT);
  pinMode(CHAN_2_INH, OUTPUT);
  pinMode(CHAN_3_INH, OUTPUT);
  pinMode(CHAN_4_INH, OUTPUT);
  pinMode(CHAN_5_INH, OUTPUT);
  pinMode(CHAN_6_INH, OUTPUT);
  pinMode(CHAN_7_INH, OUTPUT);
  pinMode(CHAN_8_INH, OUTPUT);

  pinMode(CHAN_1_IN1, OUTPUT);
  pinMode(CHAN_1_IN2, OUTPUT);
  pinMode(CHAN_2_IN1, OUTPUT);
  pinMode(CHAN_2_IN2, OUTPUT);
  pinMode(CHAN_3_IN1, OUTPUT);
  pinMode(CHAN_3_IN2, OUTPUT);
  pinMode(CHAN_4_IN1, OUTPUT);
  pinMode(CHAN_4_IN2, OUTPUT);
  pinMode(CHAN_5_IN1, OUTPUT);
  pinMode(CHAN_5_IN2, OUTPUT);
  pinMode(CHAN_6_IN1, OUTPUT);
  pinMode(CHAN_6_IN2, OUTPUT);
  pinMode(CHAN_7_IN1, OUTPUT);
  pinMode(CHAN_7_IN2, OUTPUT);
  pinMode(CHAN_8_IN1, OUTPUT);
  pinMode(CHAN_8_IN2, OUTPUT);

  pinMode(LED, OUTPUT);

  pinMode(CHANNEL_BUTTON, INPUT);
  pinMode(MODE_BUTTON, INPUT);
}

void context_init(context_t *context, int channels, ledmode mode) {
  context->channels = channels;
  context->mode = mode;
  context->unidirectional_duration_ms = 6;
  context->selected_channel = 1;
}

// C++ won't allow integer++ on enums like C89
// like captain pugwash, my true mistress is the C
void increment_ledmode(context_t *context) {
  switch (context->mode) {
    case ALL_ON:
      context->mode = SLOW_FLASH;
      break;
    case SLOW_FLASH:
      context->mode = SLOW_FLASH_RAMPED;
      break;
    case SLOW_FLASH_RAMPED:
      context->mode = FAST_FLASH;
      break;
    case FAST_FLASH:
      context->mode = RANDOM;
      break;
    case RANDOM:
      context->mode = PONG;
      break;
    case PONG:
      context->mode = CHASE;
      break;
    case CHASE:
      context->mode = PONG_RAMP;
      break;
    case PONG_RAMP:
      context->mode = CHASE_RAMP;
      break;
    case CHASE_RAMP:
      context->mode = MORSE_TEXT;
      break;
    case MORSE_TEXT:
      context->mode = ALTERNANS;
      break;
    case ALTERNANS:
      context->mode = ALL_ON;
      break;
    }
}

// this doesn't do much
void increment_channel(context_t *context) {
  context->selected_channel++;
  if (context->selected_channel == 9) {
    context->selected_channel = 1;
  }
}

void delay_us_with_polling(int delay_us, context_t *context) {
  if (delay_us <=0) {
    return;
  }
  delayMicroseconds(delay_us);
  if(!digitalRead(MODE_BUTTON)) {
    increment_ledmode(context);
    delay(25);
    return;
  }
  if(!digitalRead(CHANNEL_BUTTON)) {
    increment_channel(context);
    delay(25);
  }
}

void delay_with_polling(int delay_ms, context_t *context) {
  int half_delay = delay_ms/2;
  if (delay_ms <=0) {
    return;
  } else if (delay_ms < 3) {
    delay(delay_ms);
    return;
  }
  delay(half_delay);
  if(!digitalRead(CHANNEL_BUTTON)) {
    increment_channel(context);
    delay(25);
    return;
  }
  delay(half_delay);
  if(!digitalRead(MODE_BUTTON)) {
    increment_ledmode(context);
    delay(25);
  }
}

void enable_channels(context_t *context, int on_off) {
    if (CHANNEL1 & context->channels) {
      digitalWrite(CHAN_1_INH, on_off);
    }
    if (CHANNEL2 & context->channels) {
      digitalWrite(CHAN_2_INH, on_off);
    }
    if (CHANNEL3 & context->channels) {
      digitalWrite(CHAN_3_INH, on_off);
    }
    if (CHANNEL4 & context->channels) {
      digitalWrite(CHAN_4_INH, on_off);
    }
    if (CHANNEL5 & context->channels) {
      digitalWrite(CHAN_5_INH, on_off);
    }
    if (CHANNEL6 & context->channels) {
      digitalWrite(CHAN_6_INH, on_off);
    }
    if (CHANNEL7 & context->channels) {
      digitalWrite(CHAN_7_INH, on_off);
    }
    if (CHANNEL8 & context->channels) {
      digitalWrite(CHAN_8_INH, on_off);
    }
}

void toggle_channels(context_t *context, int pos_neg) {
    if (CHANNEL1 & context->channels) {
      digitalWrite(CHAN_1_IN1, pos_neg);
      digitalWrite(CHAN_1_IN2, 1-pos_neg);
    }
    if (CHANNEL2 & context->channels) {
      digitalWrite(CHAN_2_IN1, pos_neg);
      digitalWrite(CHAN_2_IN2, 1-pos_neg);
    }
    if (CHANNEL3 & context->channels) {
      digitalWrite(CHAN_3_IN1, pos_neg);
      digitalWrite(CHAN_3_IN2, 1-pos_neg);
    }
    if (CHANNEL4 & context->channels) {
      digitalWrite(CHAN_4_IN1, pos_neg);
      digitalWrite(CHAN_4_IN2, 1-pos_neg);
    }
    if (CHANNEL5 & context->channels) {
      digitalWrite(CHAN_5_IN1, pos_neg);
      digitalWrite(CHAN_5_IN2, 1-pos_neg);
    }
    if (CHANNEL6 & context->channels) {
      digitalWrite(CHAN_6_IN1, pos_neg);
      digitalWrite(CHAN_6_IN2, 1-pos_neg);
    }
    if (CHANNEL7 & context->channels) {
      digitalWrite(CHAN_7_IN1, pos_neg);
      digitalWrite(CHAN_7_IN2, 1-pos_neg);
    }
    if (CHANNEL8 & context->channels) {
      digitalWrite(CHAN_8_IN1, pos_neg);
      digitalWrite(CHAN_8_IN2, 1-pos_neg);
    }
}

void display_morse(context_t *context, int morse_index) {
  int i;
  for (i = 0; i < 6; i++) {
    switch (morse_lookup[morse_index].morse[i]) {
      case 1: lights_all_on_timed(context, DIT_DURATION_MS);
              delay_with_polling(DIT_DURATION_MS, context);
              break;
      case 2: lights_all_on_timed(context, DAH_DURATION_MS);
              delay_with_polling(DIT_DURATION_MS, context);
              break;
      case 0: delay_with_polling(LETTER_SPACE-DIT_DURATION_MS, context);
              return;
    }
  }
}

void char_to_morse(context_t *context, char c) {
  int morse_index;
  if (c >= 48 && c <= 57) {
    morse_index = c - 48 + 26;
    display_morse(context, morse_index);
  } else if (c >= 65 && c <= 90) {
    morse_index = c - 65;
    display_morse(context, morse_index);
  } else if (c >= 97 && c <= 122) {
    morse_index = c - 97;
    display_morse(context, morse_index);
  } else if (c == 32) {
    delay_with_polling(WORD_SPACE, context);
  }
}

void string_to_morse(context_t *context, char *text) {
  int i;
  for (i=0; text[i] != 0 && context->mode == MORSE_TEXT; i++) {
    char_to_morse(context, text[i]);
  }
}

void lights_ramped_on_off_timed(context_t *context, int duration_ms){
  ledmode mode = context->mode;
  int full_duty_microseconds = 1000;
  int on_microseconds = 5;
  int off_microseconds = full_duty_microseconds - on_microseconds;
  int ramp_steps = 200;
  // therefore ramp time equals 200ms = 200*full_duty_microseconds
  int all_on_time_ms = duration_ms - 2*ramp_steps;
  int delta_t_us = full_duty_microseconds/ramp_steps;
  int i;
  for (i = 0; i < ramp_steps; i++) {
    enable_channels(context, 1);
    toggle_channels(context, 1);
    delayMicroseconds(on_microseconds);
    enable_channels(context, 0);
    delayMicroseconds(off_microseconds);
    toggle_channels(context, 0);
    enable_channels(context, 1);
    delay_us_with_polling(on_microseconds, context);
    delayMicroseconds(on_microseconds);
    enable_channels(context, 0);
    delayMicroseconds(off_microseconds);
    on_microseconds+=delta_t_us;
    off_microseconds-=delta_t_us;
    if(context->mode != mode) {
      enable_channels(context, 0);
      break;
    }
  }
  if (all_on_time_ms > 0) {
    lights_all_on_timed(context, all_on_time_ms);
  }
  on_microseconds = 995;
  off_microseconds = full_duty_microseconds - on_microseconds;
  for (i = 1; i < ramp_steps; i++) {
    enable_channels(context, 1);
    toggle_channels(context, 1);
    delayMicroseconds(on_microseconds);
    enable_channels(context, 0);
    delayMicroseconds(off_microseconds);
    toggle_channels(context, 0);
    enable_channels(context, 1);
    delayMicroseconds(on_microseconds);
    enable_channels(context, 0);
    delay_us_with_polling(off_microseconds, context);
    on_microseconds-=delta_t_us;
    off_microseconds+=delta_t_us;
    if(context->mode != mode) {
      enable_channels(context, 0);
      break;
    }
  }
  enable_channels(context, 0);
}

void lights_ramped_on_off_timed_channel(context_t *context, int duration_ms, int channel){
  int channels_orig = context->channels;
  enable_channels(context, 0);
  context->channels = channel;
  lights_ramped_on_off_timed(context, duration_ms);
  context->channels = channels_orig;
}

void lights_all_on_timed(context_t *context, int duration_ms){
  ledmode mode = context->mode;
  int loops = duration_ms/context->unidirectional_duration_ms/2;
  int i;
  for (i = 0; i < loops; i++) {
    enable_channels(context, 1);
    toggle_channels(context, 1);
    delay_with_polling(context->unidirectional_duration_ms, context);
    toggle_channels(context, 0);
    delay_with_polling(context->unidirectional_duration_ms, context);
    if(context->mode != mode) {
      enable_channels(context, 0);
      break;
    }
  }
  enable_channels(context, 0);
}

void lights_all_on_timed_channel(context_t *context, int duration_ms, int channel){
  int channels_orig = context->channels;
  enable_channels(context, 0);
  context->channels = channel;
  lights_all_on_timed(context, duration_ms);
  context->channels = channels_orig;
}

void lights_generic_flash(context_t *context, int on_off_duration_ms, int randomise, int ramp){
  ledmode mode = context->mode;
  while (context->mode == mode){
    enable_channels(context, 1);
    if (randomise) {
      lights_all_on_timed(context, on_off_duration_ms + random(on_off_duration_ms));
    } else if (!ramp) {
      lights_all_on_timed(context, on_off_duration_ms);
    } else {
      lights_ramped_on_off_timed(context, on_off_duration_ms);
    }
    if(context->mode != mode) {
      enable_channels(context, 0);
      break;
    }
    enable_channels(context, 0);
    if (randomise) {
      delay_with_polling(on_off_duration_ms/4+random(on_off_duration_ms), context);
    } else {
      delay_with_polling(on_off_duration_ms, context);
    }
  }
}

void lights_chase_generic(context_t *context, int bounce, int ramp){
  ledmode mode = context->mode;
  int time_per_channel_ms = 500;
  while (context->mode == mode){
    if (context->channels & CHANNEL1) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL1);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL1);
      }
    }
    if(context->mode != mode) {
      break;
    }
    if (context->channels & CHANNEL2) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL2);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL2);
      }
    }
    if(context->mode != mode) {
      break;
    }
    if (context->channels & CHANNEL3) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL3);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL3);
      }
    }
    if(context->mode != mode) {
      break;
    }
    if (context->channels & CHANNEL4) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL4);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL4);
      }
    }
    if(context->mode != mode) {
      break;
    }
    if (context->channels & CHANNEL5) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL5);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL5);
      }
    }
    if(context->mode != mode) {
      break;
    }
    if (context->channels & CHANNEL6) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL6);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL6);
      }
    }
    if(context->mode != mode) {
      break;
    }
    if (context->channels & CHANNEL7) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL7);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL7);
      }
    }
    if(context->mode != mode) {
      break;
    }
    if (context->channels & CHANNEL8) {
      if (!ramp) {
        lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL8);
      } else {
        lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL8);
      }
    }
    if (bounce) {
      if (context->channels & CHANNEL7) {
        if (!ramp) {
          lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL7);
        } else {
          lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL7);
        }
      }
      if(context->mode != mode) {
        break;
      }
      if (context->channels & CHANNEL6) {
        if (!ramp) {
          lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL6);
        } else {
          lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL6);
        }
      }
      if(context->mode != mode) {
        break;
      }
      if (context->channels & CHANNEL5) {
        if (!ramp) {
          lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL5);
        } else {
          lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL5);
        }
      }
      if(context->mode != mode) {
        break;
      }
      if (context->channels & CHANNEL4) {
        if (!ramp) {
          lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL4);
        } else {
          lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL4);
        }
      }
      if(context->mode != mode) {
        break;
      }
      if (context->channels & CHANNEL3) {
        if (!ramp) {
          lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL3);
        } else {
          lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL3);
        }
      }
      if(context->mode != mode) {
        break;
      }
      if (context->channels & CHANNEL2) {
        if (!ramp) {
          lights_all_on_timed_channel(context, time_per_channel_ms, CHANNEL2);
        } else {
          lights_ramped_on_off_timed_channel(context, time_per_channel_ms, CHANNEL2);
        }
      }
    }
  }
}

void lights_alternans(context_t *context){
  context->mode = ALTERNANS;
  int on_off_duration_ms = 400;
  enable_channels(context, 1);
  while (context->mode == ALTERNANS){
    toggle_channels(context, 1);
    delay_with_polling(on_off_duration_ms, context);
    if(context->mode != ALTERNANS) {
      break;
    }
    toggle_channels(context, 0);
    delay_with_polling(on_off_duration_ms, context);
  }
}

void lights_all_on(context_t *context){
  context->mode = ALL_ON;
  do {
    enable_channels(context, 1);
    toggle_channels(context, 1);
    delay_with_polling(context->unidirectional_duration_ms, context);
    toggle_channels(context, 0);
    delay_with_polling(context->unidirectional_duration_ms, context);
  } while (context->mode == ALL_ON);
}

void lights_slow_flash(context_t *context){
  context->mode = SLOW_FLASH;
  int on_off_duration_ms = 600;
  lights_generic_flash(context, on_off_duration_ms, 0, 0);
}

void lights_slow_flash_ramped(context_t *context){
  context->mode = SLOW_FLASH_RAMPED;
  int on_off_duration_ms = 600;
  int ramp = 1;
  lights_generic_flash(context, on_off_duration_ms, 0, 1);
}

void lights_fast_flash(context_t *context){
  context->mode = FAST_FLASH;
  int on_off_duration_ms = 200;
  lights_generic_flash(context, on_off_duration_ms, 0, 0);
}

void lights_random_flash(context_t *context){
  context->mode = RANDOM;
  int on_off_duration_ms = 400;
  int randomise = 1;
  lights_generic_flash(context, on_off_duration_ms, randomise, 0);  
}

void lights_chase(context_t *context){
  context->mode = CHASE;
  int bounce = 0;
  int ramp = 0;
  lights_chase_generic(context, bounce, ramp);
}

void lights_pong(context_t *context){
  context->mode = PONG;
  int bounce = 1;
  int ramp = 0;
  lights_chase_generic(context, bounce, ramp);
}

void lights_chase_ramp(context_t *context){
  context->mode = CHASE_RAMP;
  int bounce = 0;
  int ramp = 1;
  lights_chase_generic(context, bounce, ramp);
}

void lights_pong_ramp(context_t *context){
  context->mode = PONG_RAMP;
  int bounce = 1;
  int ramp = 1;
  lights_chase_generic(context, bounce, ramp);
}

void lights_morse_text(context_t *context){  
  context->mode = MORSE_TEXT;
  while (context->mode == MORSE_TEXT){
    string_to_morse(context, MORSE_STRING);
  }
}

//builds for BluePill F103C6 (32k)
//builds for BluePill F103C8 

void loop() {
  context_t context;
  int channels = (CHANNEL1 | CHANNEL2 | CHANNEL3 | CHANNEL4 | CHANNEL5 | CHANNEL6 | CHANNEL7 | CHANNEL8);
  context_init(&context, channels, ALTERNANS);
  if (DEBUG) {
    while (1) {
      lights_alternans(&context);
      break;
    }
  }
  while(1) {
      lights_alternans(&context);
      delay(200);
      lights_all_on(&context);
      delay(200);
      lights_slow_flash(&context);
      delay(200);
      lights_slow_flash_ramped(&context);
      delay(200);
      lights_fast_flash(&context);
      delay(200);
      lights_random_flash(&context);
      delay(200);
      lights_pong(&context);
      delay(200);
      lights_chase(&context);
      delay(200);
      lights_pong_ramp(&context);
      delay(200);
      lights_chase_ramp(&context);
      delay(200);
      lights_morse_text(&context);
      delay(200);
  }
}
