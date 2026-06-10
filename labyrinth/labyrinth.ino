#include "PCA9685.h"
#include <Wire.h>
#include <Servo.h>

#define CENTER_X 100
#define CENTER_Y 95

#define START_BTN 2

#define GAME_STATE_PIN 7
#define DOORS_STATE_PIN 6
#define TRAPS_STATE_PIN 5

bool live = true;
unsigned long startTime = 0;
#define GAME_LENGTH 30000

Servo footX;
Servo footY;

ServoDriver servoBoard;

int positionX = CENTER_X;
int positionY = CENTER_X;

void setup() {
  pinMode(GAME_STATE_PIN, INPUT);
  pinMode(DOORS_STATE_PIN, INPUT);
  pinMode(TRAPS_STATE_PIN, INPUT);

  footX.attach(10);
  footY.attach(9);

  Wire.begin();
  servoBoard.init(0x7f);

  Serial.begin(9600);
}

void loop() {
  Serial.print(digitalRead(GAME_STATE_PIN));
  Serial.print(digitalRead(DOORS_STATE_PIN));
  Serial.print(digitalRead(TRAPS_STATE_PIN));

  Serial.print(digitalRead(TRAPS_STATE_PIN));
  Serial.print("\t");
  Serial.print(digitalRead(TRAPS_STATE_PIN));
  Serial.print("\t");
  Serial.println();

  updateFoot();
  updateDoors();
  updateTraps();
}

void updateDoors() {
  if (digitalRead(DOORS_STATE_PIN)) {
    openDoors();
  } else {
    closeDoors();
  }
}

void updateTraps() {
  if (digitalRead(TRAPS_STATE_PIN)) {
    openTraps();
  } else {
    closeTraps();
  }
}

void updateFoot() {
  // target position
  int x, y;

  if (!digitalRead(GAME_STATE_PIN)) {
    x = CENTER_X;
    y = CENTER_Y;
    delay(20);  // to add with the main controller delay, smoothing out the "return to idle"
  } else {
    x = CENTER_X - map(analogRead(0), 0, 1023, -10, 10);
    y = CENTER_Y - map(analogRead(1), 0, 1023, 10, -10);

    if (millis() > startTime + GAME_LENGTH) {
      live = false;
    }
  }

  if (positionX < x) {
    positionX += 1;
  } else if (positionX > x) {
    positionX -= 1;
  }

  if (positionY < y) {
    positionY += 1;
  } else if (positionY > y) {
    positionY -= 1;
  }

  footX.write(positionX);
  footY.write(positionY);
}

void openTraps() {
  servoBoard.setAngle(1, 90);
  delay(2);
  servoBoard.setAngle(4, 0);
  delay(2);
  servoBoard.setAngle(13, 90);
  delay(2);
  servoBoard.setAngle(16, 90);
  delay(2);
}

void closeTraps() {
  servoBoard.setAngle(1, 0);
  delay(2);
  servoBoard.setAngle(4, 100);
  delay(2);
  servoBoard.setAngle(13, 0);
  delay(2);
  servoBoard.setAngle(16, 12);
  delay(2);
}

void openDoors() {
  servoBoard.setAngle(5, 90);
  delay(2);
  servoBoard.setAngle(8, 150);
  delay(2);
  servoBoard.setAngle(9, 100);
  delay(2);
  servoBoard.setAngle(11, 100);
  delay(2);
}

void closeDoors() {
  servoBoard.setAngle(5, 5);
  delay(2);
  servoBoard.setAngle(8, 35);
  delay(2);
  servoBoard.setAngle(9, 0);
  delay(2);
  servoBoard.setAngle(11, 0);
  delay(2);
}