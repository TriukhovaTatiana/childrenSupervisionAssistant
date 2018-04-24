#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeAuriga.h>

//Encoder Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);

//sensors used
MeRGBLed rgbled_0(0, 12);
MeSoundSensor soundsensor_14(14);
MeLightSensor lightsensor_12(12);
MeBuzzer buzzer;
MeUltrasonicSensor ultrasonic_10(10);



double theInitial;
double arraySound[100];
double sound = 0;
int counter = 0;


double angle_rad = PI / 180.0;
double angle_deg = 180.0 / PI;
double currentTime = 0;
double lastTime = 0;

double array[3];

void turnRight();
void turnLeft();
void stopMethod();
void moveForward();
double getSound();

//method to calculate an average of sound measures
double getSound()
{
  double sum = 0;
  for (int __i__ = 0; __i__ < 100; ++__i__) {
    sound = soundsensor_14.strength();
    sum = (sum) + (sound);
    sound = 0;
    //   delay(50);
  }
  double init = sum / 100;
  return init;
}

void turnRight()
{
  lastTime = millis() / 1000.0;
  while (!((getLastTime()) > (0.6)))
  {
    _loop();
    move(4, 100);
  }

}
void turnLeft()
{
  lastTime = millis() / 1000.0;
  while (!((getLastTime()) > (0.6)))
  {
    _loop();
    move(3, 100);
  }

}

double getDistance() {
  double sum = 0;
  for (int i = 0; i < 50; i++) {
    sum = sum + ultrasonic_10.distanceCm();
  }
  return sum / 50;
}

void stopMethod()
{
  lastTime = millis() / 1000.0;

  while (!((getLastTime()) > (0.3)))
  {
    _loop();
    move(1, 0);
  }

}

void moveBackward() {
  while (!((ultrasonic_10.distanceCm()) < (10)))
  {
    _loop();
    move(2, 100);
  }
}

void moveForward()
{

  while (!((ultrasonic_10.distanceCm()) < (45)))
  {
    _loop();
    move(1, 255);
    if (ultrasonic_10.distanceCm() < 10) {
      moveBackward();
    }
  }
}


double getLastTime() {
  return currentTime = millis() / 1000.0 - lastTime;
}



void isr_process_encoder1(void)
{
  if (digitalRead(Encoder_1.getPortB()) == 0) {
    Encoder_1.pulsePosMinus();
  }
  else {
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(void)
{
  if (digitalRead(Encoder_2.getPortB()) == 0) {
    Encoder_2.pulsePosMinus();
  }
  else {
    Encoder_2.pulsePosPlus();
  }
}

void move(int direction, int speed)
{
  int leftSpeed = 0;
  int rightSpeed = 0;
  if (direction == 1) {
    leftSpeed = -speed;
    rightSpeed = speed;
  } else if (direction == 2) {
    leftSpeed = speed;
    rightSpeed = -speed;
  } else if (direction == 3) {
    leftSpeed = -speed;
    rightSpeed = -speed;
  } else if (direction == 4) {
    leftSpeed = speed;
    rightSpeed = speed;
  }
  Encoder_1.setTarPWM(leftSpeed);
  Encoder_2.setTarPWM(rightSpeed);
}

void moveDegrees(int direction, long degrees, int speed_temp)
{
  speed_temp = abs(speed_temp);
  if (direction == 1)
  {
    Encoder_1.move(-degrees, (float)speed_temp);
    Encoder_2.move(degrees, (float)speed_temp);
  }
  else if (direction == 2)
  {
    Encoder_1.move(degrees, (float)speed_temp);
    Encoder_2.move(-degrees, (float)speed_temp);
  }
  else if (direction == 3)
  {
    Encoder_1.move(-degrees, (float)speed_temp);
    Encoder_2.move(-degrees, (float)speed_temp);
  }
  else if (direction == 4)
  {
    Encoder_1.move(degrees, (float)speed_temp);
    Encoder_2.move(degrees, (float)speed_temp);
  }
}


void setup() {
  rgbled_0.setpin(44);
  rgbled_0.setColor(0, 60, 20, 150);
  rgbled_0.show();
  rgbled_0.setColor(0, 0, 0, 0);
  rgbled_0.show();

  Serial.begin(9600);
  delay(100);

  while (theInitial < 163 || theInitial >= 165) {
    theInitial = getSound();
  }
  Serial.println(theInitial);
  // }
  for (int i = 0; i < 100; i++) {
    arraySound[i] = getSound();
    delay(70);

    attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
    attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
    Encoder_1.setPulse(9);
    Encoder_1.setRatio(39.267);
    Encoder_1.setPosPid(1.8, 0, 1.2);
    Encoder_1.setSpeedPid(0.18, 0, 0);
    Encoder_2.setPulse(9);
    Encoder_2.setRatio(39.267);
    Encoder_2.setPosPid(1.8, 0, 1.2);
    Encoder_2.setSpeedPid(0.18, 0, 0);

    //Set Pwm 8KHz
    TCCR1A = _BV(WGM10);
    TCCR1B = _BV(CS11) | _BV(WGM12);
    TCCR2A = _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS21);


  }

  void navigateHouse() {

    stopMethod();
    turnLeft();
    stopMethod();
    array[0] = getDistance();

    turnRight();
    stopMethod();
    array[1] = getDistance();

    turnRight();
    array[2] = getDistance();

    stopMethod();

    //going in left direction
    if (array[0] > array[1] && array[0] > array[2]) {
      turnLeft();
      turnLeft();
      stopMethod();
      if ((array[0]) < (35)) {
        turnLeft();
      }
      else {
        moveForward();
      }

      navigateHouse();
    }

    if (array[1] > array[0] && array[1] < array[2]) {
      turnLeft();
      stopMethod();
      if ((array[1]) < (35)) {
        turnRight();
        turnRight();
      }
      else {
        moveForward();
      }
      // stopMethod();
      navigateHouse();
    }

    if (array[2] > array[0] && array[2] > array[1]) {
      if ((array[2]) < (35)) {
        turnRight();
      }
      else {
        moveForward();
      }
      //stopMethod();
      navigateHouse();
    }
  }
  void methodSound() {
    checkForSound(3.0, 80);
    // buzzer.setpin(45);
    //  buzzer.tone(262, 500);
    for (int a = 99; a > 0; a--) {
      arraySound[a] = arraySound[a - 1];
    }
    arraySound[0] = getSound();
    Serial.println(arraySound[0]);

    delay(100);
  }

  void loop() {
    if (Serial.available()) {
      char val = Serial.read();
      if (val == 'a') {
        methodSound();
      }
    }
  }
  /*  if(val == 'b'){
      guard();
    }
    if(val=='c'){
           Serial.println("Hello");

    }
    if(val='d'){
       buzzer.tone(262, 500);
        buzzer.tone(262, 500);
         buzzer.tone(262, 500);
    }
    }*/

  //navigateHouse();




  void guard() {
    double initDistance = getDistance();
    delay(100);
    double newDistance = getDistance();
    if (newDistance > initDistance + 2) {
      while (newDistance > initDistance + 2) {
        buzzer.tone(262, 500);
      }
    }
  }

  void checkForSound(double increment, int compare) {
    counter = 0;
    for (int i = 0; i < 100; i++)
    {
      if ((arraySound[i]) > (theInitial + 2.0))
      {
        counter = counter + 1;
        Serial.println(counter);
      }
    }
    if (counter > 25) {
      if (((lightsensor_12.read()) == (0))) {
        rgbled_0.setColor(0, 150, 60, 255);
        rgbled_0.show();
        exit(0);
      }
      else {


        navigateHouse();
      }

    }


  }

  void _delay(float seconds) {
    long endTime = millis() + seconds * 1000;
    while (millis() < endTime)_loop();
  }

  void _loop() {
    Encoder_1.loop();
    Encoder_2.loop();


  }

