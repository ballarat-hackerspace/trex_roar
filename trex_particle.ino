


/*
 ******************************************************************************
 *  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */
 
/* HC-SR04 Ping / Range finder wiring:
 * -----------------------------------
 * Particle - HC-SR04
 *      GND - GND
 *      VIN - VCC
 *       D2 - TRIG
 *       D6 - ECHO
 */
 
#include "application.h"


// Derived from https://github.com/eltallerdiminuto/spark_HC-SR04_ultrasonic_sensor/blob/master/HC-SR04.ino

int VCC = D5;
int TRIG = D6;
int ECHO = D7;
int LED = A0;
int IND = A5;  // Indicator


int time_last_roared = 0;
int MIN_TIME_ROAR = 5 * 60;  // Five minutes
int is_active = 0;  // This increments on each loop where the sensor is active. It needs NUM_ACTIVE continuous iterations to activate. This reduces some noise
int NUM_ACTIVE = 3;

void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
}

void loop() {
    // Trigger pin, Echo pin, delay (ms), visual=true|info=false
    ping(TRIG, ECHO, 20, true);
}

void roar(){
    if (Time.now() - time_last_roared > MIN_TIME_ROAR){
        Particle.publish("roar");
        time_last_roared = Time.now();
    }
    
}

void ping(pin_t trig_pin, pin_t echo_pin, uint32_t wait, bool info)
{
    uint32_t duration, inches, cm;
    static bool init = false;
    if (!init) {
        pinMode(trig_pin, OUTPUT);
        digitalWriteFast(trig_pin, LOW);
        pinMode(echo_pin, INPUT);
        delay(50);
        init = true;
    }

    /* Trigger the sensor by sending a HIGH pulse of 10 or more microseconds */
    digitalWriteFast(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWriteFast(trig_pin, LOW);
  
    duration = pulseIn(echo_pin, HIGH);
    
    /* Convert the time into a distance */
    // Sound travels at 1130 ft/s (73.746 us/inch)
    // or 340 m/s (29 us/cm), out and back so divide by 2
    // Ref: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
    cm = duration / 29 / 2;
    if (cm < 20){
        is_active += 1;
        if (is_active >= NUM_ACTIVE){
            roar();
            is_active = 0;
        }
        digitalWriteFast(LED, HIGH);
    }else{
        is_active = 0;
        digitalWriteFast(LED, LOW);
    }
      
    if (info) { /* Visual Output */
        Serial.printf("%2d:", inches);
        for(int x=0;x<inches;x++) Serial.print("#");
        Serial.println();
    } else { /* Informational Output */
        Serial.printlnf("%6d in / %6d cm / %6d us", inches, cm, duration);
    }
    delay(wait); // slow down the output
}
