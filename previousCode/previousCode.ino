#include <SimpleTimer.h>
/*This code was forked from github repo
  https://github.com/jhajek/ITMT492-SPRING2015/blob/master/Clothing-Based-Proximity-Sensors-for-the-Visually-Impaired/Final_Embeded/programCode.txt */

#define SIGNIFICANT_DISTANCE 50 //THIS IS HOW FAR DETECTION WILL CHANGE BEFORE WE UPDATE FREQUENCY TIMER
#define maxDistance 200
SimpleTimer timer;
int sensorsPin[3] = {10, 11, 12}; //Pins for each sensor
int vibrationMotorPins[3] = {4, 5, 6}; //Pins for vibration motors - Have to be PWM pins.
int distanceLevel[3]; //Array of frequencies for each side
volatile boolean on = true; //if touchpad is touched, switch this. if false, shut down vibrators.
long pingDistanceArray[3][20];
int pingCounter[3];
int pause[3];
long distanceSum[3];
int pointer = 0;
int samples = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 3; i++)
    pinMode(vibrationMotorPins[i], OUTPUT);
  pinMode(13, OUTPUT);
  timer.setInterval(60, vibrateAll);
}
long old;
void loop() {
  Serial.print("d:" + (String)(millis() - old) + "  ");
  old = millis();

  pointer = pointer > 2 ? 0 : pointer;
  pingDistance(pointer++);
  if (pointer == 3)
    Serial.println();

  timer.run();
}

//This function chcks all frequencies and vibrates according to the frequency values
void vibrateAll() {
  //timer (every 10 msec, if freq=0, vibrating, else not)
  for (int i = 0; i < 3; i++)
    vibrate(i);
}


//pulses for only 20ms, then pauses
// Vibrate if system is on, acording to the frequency
void vibrate(int side) {
  if (!tooFar(side)) {
    //turn on, decrease by 1
    if (pause[side] == distanceLevel[side]) {
      digitalWrite(vibrationMotorPins[side], HIGH);
      pause[side]--;
    } else if (pause[side] < distanceLevel[side]) {
      digitalWrite(vibrationMotorPins[side], LOW);
      pause[side]--;
    }
    if (pause[side] <= -1 || pause[side] >= distanceLevel[side]) {
      pause[side] = distanceLevel[side];
    }
  } else {
    digitalWrite(vibrationMotorPins[side], LOW);
  }

}


//If change in distance is significant, then set frequency (distanceLevel)
void pingDistance(int side) {
  long distance =  getDistance(sensorsPin[side]);
  distanceSum[side] -= pingDistanceArray[side][pingCounter[side]];
  distanceSum[side] += distance;
  pingDistanceArray[side][pingCounter[side]] = distance;

  //ping counter is pingDistanceArray position
  if (pingCounter[side] < samples - 1) {
    pingCounter[side]++;
  } else {
    pingCounter[side] = 0;
  }
  Serial.print((String)averageDistance(side) + "   ");
  //now, see if we need to start vibrating, and if so, begin the vibrate timer for this side.
  if (!tooFar(side)) {
    distanceLevel[side] = averageDistance(side) / SIGNIFICANT_DISTANCE;
  }
}

//Check if ditance to object is irrelevant
bool tooFar(int side) {
  if (averageDistance(side) <= maxDistance && averageDistance(side) > 0) {
    return false;
  }
  return true;
}

long averageDistance (int side) {
  return distanceSum[side] / samples;
}

//Do Ping for a pin and return distance based on the response time
long getDistance(int pingPin) {
  pinMode(pingPin, INPUT);

  long duration = pulseIn(pingPin, HIGH);
  long cm = duration * 2.54 / 147;
  return cm;
}

