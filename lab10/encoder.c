#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"
#include "lcd.h"

int temp_diff = 0;
char changed = 0;
char old_state, new_state;
char a, b;

void encoder_init() {
  // Read the A and B inputs to determine the intial state
  unsigned char input = PINC;
  a = input & (1 << PC1);
  b = input & (1 << PC5);
  if (a && b)
    old_state = 3;
  else if (!a && !b)
    old_state = 0;
  else if (!a && b)
    old_state = 2;
  else
    old_state = 1;
}

int get_temp() {
  return temp_diff;
}

void reset_temp() {
  temp_diff = 0;
}

void has_change() {
  if(changed == 1) {
    changed = 0;
    return 1;
  }
  else
    return 0;
}

ISR(PCINT1_vect) {
  // Read the input bits and determine A and B
  unsigned char input = PINC;
  a = input & (1 << PC1);
  b = input & (1 << PC5);

  // For each state, examine the two input bits to see if state
  // has changed, and if so set "new_state" to the new state,
  // and adjust the temp value.
  if (old_state == 0) {
    // Handle A and B inputs for state 0
    if (a) {
      new_state = 1;
      temp_diff++;
    }
    else if (b) {
      new_state = 2;
      temp_diff--;
    }
  }
  else if (old_state == 1) {
    // Handle A and B inputs for state 1
    if (!a) {
      new_state = 0;
      temp_diff--;
    }
    else if (b) {
      new_state = 3;
      temp_diff++;
    }
  }
  else if (old_state == 2) {
    // Handle A and B inputs for state 2
    if (a) {
      new_state = 3;
      temp_diff--;
    }
    else if (!b) {
      new_state = 0;
      temp_diff++;
    }
  }
  else {   // old_state = 3
    // Handle A and B inputs for state 3
    if (!b) {
      new_state = 1;
      temp_diff--;
    }
    else if (!a) {
      new_state = 2;
      temp_diff++;
    }
  }

  // If state changed, update the value of old_state
  if (new_state != old_state) {
    old_state = new_state;
    changed = 1;
  }
}