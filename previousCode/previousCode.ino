#include <SimpleTimer.h>
/*This code was forked from github repo
  https://github.com/jhajek/ITMT492-SPRING2015/blob/master/Clothing-Based-Proximity-Sensors-for-the-Visually-Impaired/Final_Embeded/programCode.txt */

#define SIGNIFICANT_DISTANCE 50 // The distance value between vibration levels
#define maxDistance 200 // The maximum distance to consider obstacles
SimpleTimer timer; // Timer to synchronize the frequency of the vibration motors
int sensorsPin[3] = {6, 11, 5}; // Pins for each sensor
int vibrationMotorPins[3] = {7, 12, 4}; // Pins for vibration motors
int distance[3]; // Distances from each sensor
int distanceLevel[3]; // Array of frequencies for each side
int pause[3]; // Variable used to synchronize the frequency of the vibration motors
int pointer; // Variable to iterate sensors between loops

void setup() {
  // Declare the vibration motors as output pins
  for (int i = 0; i < 3; i++)
    pinMode(vibrationMotorPins[i], OUTPUT);
  timer.setInterval(60, vibrateAll); // Set the period of vibrations
}

void loop() {
  pingDistance(pointer++ % 3); // Check the distance of a sensor
  timer.run(); // Vibrate all motors
}

// This function checks all frequencies and vibrates according to the frequency values
void vibrateAll() {
  for (int i = 0; i < 3; i++)
    vibrate(i); // Vibrate each of the three motors
}

// Vibrate if object is inside the maximum distance, acording to the frequency
// Vibrates once and pauses for ((maxDistance / SIGNIFICANT_DISTANCE) - 1) periods, then restarts
void vibrate(int side) {
  if (!tooFar(side)) { // Verify if it is inside the maximum distance
    if (pause[side] == distanceLevel[side]) { // Vibrates once
      digitalWrite(vibrationMotorPins[side], HIGH);
      pause[side]--;
    } else if (pause[side] < distanceLevel[side]) { // Pauses the vibration for the other periods
      digitalWrite(vibrationMotorPins[side], LOW);
      pause[side]--;
    }
    if (pause[side] <= -1 || pause[side] >= distanceLevel[side]) { // Restarts the pause counter
      pause[side] = distanceLevel[side];
    }
  } else { // Power off the motor if there is no obstacle in the maximum distance range
    digitalWrite(vibrationMotorPins[side], LOW);
  }
}


// If change in distance is significant, then set frequency (distanceLevel)
// Trigger each sensor, checks if it is inside the maximum distance range, and define the distanceLevel
void pingDistance(int side) {
  distance[side] =  getDistance(sensorsPin[side]);
  if (!tooFar(side)) {
    distanceLevel[side] = distance[side] / SIGNIFICANT_DISTANCE;
  }
}

// Sends a pulse to the sensor and return the distance in centimeters based on the response time from the sensor
long getDistance(int pingPin) {
  pinMode(pingPin, INPUT);
  long duration = pulseIn(pingPin, HIGH);
  long cm = duration * 2.54 / 147;
  return cm;
}

// Check if ditance to object is irrelevant
bool tooFar(int side) {
  if (distance[side] <= maxDistance && distance[side] > 0) {
    return false;
  }
  return true;
}

