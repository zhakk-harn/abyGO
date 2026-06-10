#include <Adafruit_NeoPixel.h>

#define START_BTN 2
#define DOORS_BTN 3
#define TRAPS_BTN 4

#define GAME_STATE_PIN 12
#define DOORS_STATE_PIN 11
#define TRAPS_STATE_PIN 10

#define DOORS_COOLDOWN 3000
#define TRAPS_DURATION 3000
#define TRAPS_COOLDOWN 3000
#define GAME_DURATION 60000

bool startBtnPressed = false;
bool doorsBtnPressed = false;
bool trapsBtnPressed = false;

bool gameState = false;
bool doorsState = false;
bool trapsState = false;

unsigned long doorsTimer = 0;
unsigned long trapsTimer = 0;
unsigned long lastDoorChange = 0;
unsigned long lastTrapChange = 0;
unsigned long gameStartTime = 0;

enum State {
  IDLE,
  RUNNING,
  GAMEOVER
};

enum State state = IDLE;

Adafruit_NeoPixel doorsPixels(16, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel trapsPixels(16, 7, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ambiantPixels(50, 8, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(DOORS_BTN, INPUT_PULLUP);
  pinMode(TRAPS_BTN, INPUT_PULLUP);

  pinMode(GAME_STATE_PIN, OUTPUT);
  pinMode(DOORS_STATE_PIN, OUTPUT);
  pinMode(TRAPS_STATE_PIN, OUTPUT);

  doorsPixels.begin();
  trapsPixels.begin();
  ambiantPixels.begin();

  doorsPixels.clear();
  trapsPixels.clear();
  ambiantPixels.clear();

  doorsPixels.show();
  trapsPixels.show();
  ambiantPixels.show();

  Serial.begin(9600);
}

void loop() {
  switch (state) {
    case IDLE:
      idle();
      break;
    case RUNNING:
      running();
      break;
    case GAMEOVER:
      gameOver();
      break;
  }

  digitalWrite(GAME_STATE_PIN, gameState);
  digitalWrite(DOORS_STATE_PIN, doorsState);
  digitalWrite(TRAPS_STATE_PIN, trapsState);

  Serial.print(gameState);
  Serial.print("\t");
  Serial.print(doorsState);
  Serial.print("\t");
  Serial.print(trapsState);
  Serial.println();
}

void idle() {
  if (digitalRead(START_BTN) == HIGH) {
    startBtnPressed = false;
  }

  if (digitalRead(START_BTN) == LOW && !startBtnPressed) {
    startBtnPressed = true;
    gameStartTime = millis();
    state = RUNNING;
    gameState = true;
  }

  trapsPixels.clear();
  doorsPixels.clear();
  trapsPixels.show();
  doorsPixels.show();

  // TODO animation d'attract mode sur les LEDS
}

void running() {
  if (digitalRead(DOORS_BTN) == HIGH) {
    doorsBtnPressed = false;
  }

  if (digitalRead(DOORS_BTN) == LOW && !doorsBtnPressed) {
    doorsBtnPressed = true;

    if (millis() > lastDoorChange + DOORS_COOLDOWN) {
      doorsState = !doorsState;
      lastDoorChange = millis();
    }
  }

  if (digitalRead(TRAPS_BTN) == HIGH) {
    trapsBtnPressed = false;
  }

  if (digitalRead(TRAPS_BTN) == LOW && !trapsBtnPressed) {
    trapsBtnPressed = true;

    if (millis() > lastTrapChange + TRAPS_DURATION) {
      trapsState = true;
      lastTrapChange = millis();
    }
  }

  if (trapsState && millis() > lastTrapChange + TRAPS_DURATION) {
    trapsState = false;
  }

  if (millis() > gameStartTime + GAME_DURATION) {
    state = GAMEOVER;
    doorsState = false;
    trapsState = false;
    gameState = false;
  }

  trapsPixels.clear();
  doorsPixels.clear();

  // TODO there's probably a problem with the variable casting which makes the ring flashes at the end ¯\_(ツ)_/¯
  if (lastDoorChange + DOORS_COOLDOWN > millis()) {
    float progress = 1.0f - (float)(millis() - lastDoorChange) / DOORS_COOLDOWN;
    doorsPixels.fill(doorsPixels.Color(100, 0, 0), 0, progress * doorsPixels.numPixels());
  }

  // TODO there's probably a problem with the variable casting which makes the ring flashes at the end ¯\_(ツ)_/¯
  if (lastTrapChange + TRAPS_COOLDOWN + TRAPS_DURATION > millis()) {
    float progress = 1.0f - (float)(millis() - lastTrapChange) / (TRAPS_COOLDOWN + TRAPS_DURATION);
    trapsPixels.fill(trapsPixels.Color(100, 0, 0), 0, progress * trapsPixels.numPixels());
  }

  trapsPixels.show();
  doorsPixels.show();
}

void gameOver() {
  // animate the LEDs with delays and go back to idle once the animation is done
  // make it short so we can restart a game quickly
  delay(1000);
  state = IDLE;
}