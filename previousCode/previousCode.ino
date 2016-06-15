#include <SimpleTimer.h>
/*This code was forked from github repo
https://github.com/jhajek/ITMT492-SPRING2015/blob/master/Clothing-Based-Proximity-Sensors-for-the-Visually-Impaired/Final_Embeded/programCode.txt */

#define SIGNIFICANT_DISTANCE 10 //THIS IS HOW FAR DETECTION WILL CHANGE BEFORE WE UPDATE FREQUENCY TIMER
SimpleTimer timer;
int buttonPin = 11; //Button Pin
int sensorsPin[3] = {12, 4, 6}; //Pins for each sensor
int vibrationMotorPins[3] = {3, 5, 7}; //Pins for vibration motors
int distanceLevel[3]; //Array of frequencies for each side
boolean on = false; //if touchpad is touched, switch this. if false, shut down vibrators.
boolean tooFar[] = {true, true, true}; //if too far from sensor, don't attempt to vibrate
long pingDistanceArray[3][20];
int pingCounter[3];
int pause[3];

boolean buttonState = 0; //True if button is pressed

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(vibrationMotorPins[0], OUTPUT);
  pinMode(vibrationMotorPins[1], OUTPUT);
  pinMode(vibrationMotorPins[2], OUTPUT);
  timer.setInterval(20, vibrateAll);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (on) {
  pingDistance(0);
  pingDistance(1);
  pingDistance(2);
  }
  checkTouchpad();
  timer.run();
  delay(3);
}

//This function chcks all frequencies and vibrates according to the frequency values
void vibrateAll() { 
  //timer (every 10 msec, if freq=0, vibrating, else not)
  vibrate(0);
  vibrate(1);
  vibrate(2);
}

//This function checks if button is pressed and turns system on/of accordingly
void checkTouchpad(){
  boolean newState = (digitalRead(buttonPin) == HIGH) ? 1 : 0;
  if(newState != buttonState && newState == 1){
   on = !on;
  }
  buttonState = newState;
}


//pulses for only 20ms, then pauses
// Vibrate if system is on, acording to the frequency
void vibrate(int side) {
  if (!tooFar[side] && on) {
    if (pause[side] == distanceLevel[side]) {
      digitalWrite(vibrationMotorPins[side], HIGH);
      pause[side]--;
    } else if (pause[side] < distanceLevel[side]) {
      digitalWrite(vibrationMotorPins[side], LOW);
      pause[side]--;
    }
    if (pause[side] <= 0 || pause[side] >= distanceLevel[side]) {
      pause[side] = distanceLevel[side];
    }
  } else {
    digitalWrite(vibrationMotorPins[side], LOW);
  }
}


//If change in distance is significant, then set frequency (distanceLevel)
void pingDistance(int side) {
  long distance =  getDistance(sensorsPin[side]);

  pingDistanceArray[side][pingCounter[side]] = distance;
  long avgDistance = averageDistance(pingDistanceArray[side]);
  int distanceValue;

  //ping counter is pingDistanceArray position
  if (pingCounter[side] < 19) {
    pingCounter[side]++;
  } else {
    pingCounter[side] = 0;
  }

  //now, see if we need to start vibrating, and if so, begin the vibrate timer for this side.
  if (avgDistance <= 70 && avgDistance > 0) {
    distanceValue = avgDistance / SIGNIFICANT_DISTANCE;
    distanceLevel[side] = distanceValue;
    tooFar[side] = false;
  } else {
    tooFar[side] = true;
  }
}

long averageDistance (long distanceArray[]) {
  long total = 0;
  for (int i = 0; i < 20; i++) {
    total += distanceArray[i];
  }
  return (total / 20);
}

//Do Ping for a pin and return distance in cm, based on the response time
long getDistance(int pingPin) {
  long pulse = pulseIn(pingPin, HIGH);
  long cm = (pulse / 147) * 2.54;
  return cm;
}

