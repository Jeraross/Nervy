#include "Ultrasonic.h"
#include <LiquidCrystal.h>

// Define LCD pins (adjust these according to your wiring)
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Ultrasonic ultra1(8, 9);
Ultrasonic ultra2(10, 11);
Ultrasonic ultra3(12, 13);
Ultrasonic ultra4(14, 15);

const int led1 = A0;
const int led2 = A1;
const int led3 = A2;
const int led4 = A3;

const int btnPin = A4;
bool isPressed = false;
bool inicioJogo = false;

unsigned long inicio;
unsigned long tempo = 5000;
unsigned long lastActivationTime; // To track time between sensor activations

int score = 0;
int level = 1;

// Updated Points Needed for Level Progression
int scoreToNextLevel = 200; // Points needed for Level 2
int activeSensors = 1; // Number of sensors activated at once

const unsigned long bonusWindow = 300; // 0,3 - second window for the bonus

void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2); // Initialize 16x2 LCD
  lcd.setCursor(0, 0);
  lcd.print("Seja Rapido,");
  lcd.setCursor(0, 1);
  lcd.print("Seja NERVY!");

  pinMode(btnPin, INPUT_PULLUP);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  randomSeed(analogRead(A5));
}

void loop() {
  if (digitalRead(btnPin) == LOW && !isPressed) {
    isPressed = true;
    inicioJogo = true;
    inicio = millis();
    tempo = 5000; // Reset the initial time for Easy level
    score = 0;
    level = 1;
    activeSensors = 1; // Start with one sensor for Easy

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level: Easy");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);

    delay(500); // Start delay
  } else if (digitalRead(btnPin) == HIGH && isPressed) {
    isPressed = false;
  }

  if (inicioJogo) {
    playLevel();
  }
}

void playLevel() {
  int sensorsToActivate[4] = {LOW, LOW, LOW, LOW}; // Initialize sensors to LOW
  for (int i = 0; i < activeSensors; i++) {
    int opcao;
    do {
      opcao = random(1, 5); // Pick a random sensor to activate
    } while (sensorsToActivate[opcao - 1] == HIGH); // Ensure unique activation
    sensorsToActivate[opcao - 1] = HIGH;
  }

  digitalWrite(led1, sensorsToActivate[0]);
  digitalWrite(led2, sensorsToActivate[1]);
  digitalWrite(led3, sensorsToActivate[2]);
  digitalWrite(led4, sensorsToActivate[3]);

  // Monitor the sensor activations by the player
  bool sensoresAcertados[4] = {false, false, false, false}; // Array to track which sensors were correctly activated
  bool acertou = false;

  while (!acertou) {
    // Check each sensor individually, and mark as acertado if within range
    if (sensorsToActivate[0] && ultra1.read() <= 15) {
      sensoresAcertados[0] = true;
    }
    if (sensorsToActivate[1] && ultra2.read() <= 15) {
      sensoresAcertados[1] = true;
    }
    if (sensorsToActivate[2] && ultra3.read() <= 15) {
      sensoresAcertados[2] = true;
    }
    if (sensorsToActivate[3] && ultra4.read() <= 15) {
      sensoresAcertados[3] = true;
    }

    // Check if all the activated sensors were acertados
    if (sensorsActivatedCorrectly(sensorsToActivate, sensoresAcertados)) {
      acertou = true; // All activated sensors were acertados
      apagarLeds();  // Apaga todos os LEDs antes de processar a ativação
      processActivation(); // Call processActivation only after all activated sensors are acertados
    }

    if (millis() - inicio >= tempo) {
      endGame();
      return;
    }
  }

  // Check if we need to level up based on score
  checkLevelUp();

  // Update LCD with score info
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

// Function to check if all activated sensors were correctly triggered
bool sensorsActivatedCorrectly(int sensorsToActivate[], bool sensoresAcertados[]) {
  for (int i = 0; i < 4; i++) {
    if (sensorsToActivate[i] != sensoresAcertados[i]) {
      return false;
    }
  }
  return true;
}

void processActivation() {
  unsigned long currentActivationTime = millis();
  int pointsGained = 10; // Default points for a single activation

  // Check if player activated the sensor within the bonus window
  if (currentActivationTime - lastActivationTime <= bonusWindow) {
    pointsGained += 10; // Bonus points for fast consecutive activation
  }

  score += pointsGained; // Update score with possible bonus
  inicio = millis(); // Reset the timer for the next round

  // Decrease time as difficulty increases
  tempo -= 50;
  if (tempo < 500) tempo = 500; // Minimum limit for time

  lastActivationTime = currentActivationTime;
}

void checkLevelUp() {
  // Level up based on score
  if (score >= scoreToNextLevel) {
    if (level == 1) {
      // Level 2: Medium difficulty
      levelUp("Medium", 2, 1500, 2, 500); // Next level requires 500 points
    } else if (level == 2) {
      // Level 3: Hard difficulty, continues until the player loses
      levelUp("Hard", 3, 1000, 3, -1); // No further level, game continues
    }
  }
}

void levelUp(String levelName, int newLevel, unsigned long newTempo, int newActiveSensors, int nextScoreThreshold) {
  level = newLevel;
  tempo = newTempo;
  activeSensors = newActiveSensors;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(levelName);
  
  delay(2000); // Small delay to allow player to see the level change

  // Update the score threshold for the next level, if applicable
  if (nextScoreThreshold > 0) {
    scoreToNextLevel = nextScoreThreshold;
  }
}

void apagarLeds() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}

void endGame() {
  inicioJogo = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score Final: ");
  lcd.print(score);

  for (int i = 0; i < 5; i++) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, LOW);
    delay(200);

    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
    delay(200);
  }
}
