#include <SimpleTimer.h>
/*This code was forked from github repo
https://github.com/jhajek/ITMT492-SPRING2015/blob/master/Clothing-Based-Proximity-Sensors-for-the-Visually-Impaired/Final_Embeded/programCode.txt */

#define SIGNIFICANT_DISTANCE 10 //THIS IS HOW FAR DETECTION WILL CHANGE BEFORE WE UPDATE FREQUENCY TIMER
SimpleTimer timer;
int buttonPin = 2; //Button Pin
int sensorsPin[3] = {10, 11, 12}; //Pins for each sensor
int vibrationMotorPins[3] = {3, 5, 6}; //Pins for vibration motors - Have to be PWM pins.
int distanceLevel[3]; //Array of frequencies for each side
boolean on = false; //if touchpad is touched, switch this. if false, shut down vibrators.
long pingDistanceArray[3][20];
int pingCounter[3];
int pause[3];
long distanceSum[3];
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
  if (!tooFar(side) && on) {
    /*if (pause[side] == distanceLevel[side]) {
      digitalWrite(vibrationMotorPins[side], HIGH);
      pause[side]--;
    } else if (pause[side] < distanceLevel[side]) {
      digitalWrite(vibrationMotorPins[side], LOW);
      pause[side]--;
    }
    if (pause[side] <= 0 || pause[side] >= distanceLevel[side]) {
      pause[side] = distanceLevel[side];
    }*/
    int vibeVal = map(averageDistance(side), 0, 70, 255, 0);
    analogWrite(vibrationMotorPins[side], vibeVal);
  } else {
    analogWrite(vibrationMotorPins[side], LOW);
  }
}


//If change in distance is significant, then set frequency (distanceLevel)
void pingDistance(int side) {
  long distance =  getDistance(sensorsPin[side]);
  distanceSum[side] -= pingDistanceArray[side][pingCounter[side]];
  distanceSum[side] += distance;
  pingDistanceArray[side][pingCounter[side]] = distance;
  long avgDistance = averageDistance(side);
  int distanceValue;

  //ping counter is pingDistanceArray position
  if (pingCounter[side] < 19) {
    pingCounter[side]++;
  } else {
    pingCounter[side] = 0;
  }

  //now, see if we need to start vibrating, and if so, begin the vibrate timer for this side.
  /*if (avgDistance <= 70 && avgDistance > 0) {
    distanceValue = avgDistance / SIGNIFICANT_DISTANCE;
    distanceLevel[side] = distanceValue;
  } */
}

//Check if ditance to object is irrelevant
bool tooFar(int side) {
  if (averageDistance(side) <= 70 && averageDistance(side) > 0) {
    return false;
  }
  return true;
}

long averageDistance (int side) {
  return distanceSum[side] / 20;
}

//Do Ping for a pin and return distance based on the response time
long getDistance(int pingPin) {
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(pingPin, INPUT);
  
  int duration = pulseIn(pingPin, HIGH, 5000);
  long cm = microsecondsToCentimeters(duration);
  return cm;
}


//calculates distance based on microseconds gathered from Ping
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
