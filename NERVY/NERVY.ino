#include <LiquidCrystal_I2C.h>
#include "Ultrasonic.h"

// Inicializando o LCD com o endereço I2C e as dimensões (16 colunas e 2 linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Verifique se o endereço I2C do seu módulo é 0x27 ou 0x3F

Ultrasonic ultra1(6, 7);
Ultrasonic ultra2(8, 9);
Ultrasonic ultra3(10, 11);
Ultrasonic ultra4(12, 13);

const int led1 = 5;
const int led2 = 2;
const int led3 = 3;
const int led4 = 4;

int leds[] = {led1, led2, led3, led4};

const int buzzerPin = A2;
bool isPlaying = false;

const int btnPin = A0;
bool isPressed = false;
bool inicioJogo = false;

unsigned long inicio;
unsigned long tempo = 2500;
unsigned long lastActivationTime;

int score = 0;
int level = 1;

int scoreToNextLevel = 400;
int activeSensors = 1;

const unsigned long bonusWindow = 400;

void setup() {
  Serial.begin(9600);
  
  lcd.init();            // Inicializa o LCD
  lcd.backlight();       // Liga a luz de fundo do LCD
  
  lcd.setCursor(0, 0);
  lcd.print("Seja Rapido.");
  lcd.setCursor(0, 1);
  lcd.print("Seja NERVY!");

  pinMode(btnPin, INPUT_PULLUP);

  pinMode(buzzerPin, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
  }

  randomSeed(analogRead(A1));
}

void loop() {
  if (digitalRead(btnPin) == LOW && !isPressed) {
    isPressed = true;
    inicioJogo = true;
    tempo = 2500;
    score = 0;
    level = 1;
    activeSensors = 1; 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1 ");
    tone(buzzerPin, 1000);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    delay(500);
    noTone(buzzerPin);
    delay(500);

    lcd.print("2 ");
    tone(buzzerPin, 1000);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    delay(500);
    noTone(buzzerPin);
    delay(500);

    lcd.print("3 ");
    tone(buzzerPin, 1000);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, LOW);
    delay(500);
    noTone(buzzerPin);
    delay(500);

    lcd.setCursor(0, 0);
    lcd.print("NERVY!");
    tone(buzzerPin, 1000);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
          digitalWrite(leds[j], HIGH);
        }
        delay(50);
        for (int j = 0; j < 4; j++) {
          digitalWrite(leds[j], LOW);
        }
        delay(50);
      }
    noTone(buzzerPin);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
          digitalWrite(leds[j], HIGH);
        }
        delay(50);
        for (int j = 0; j < 4; j++) {
          digitalWrite(leds[j], LOW);
        }
        delay(50);
      }

    inicio = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level: Easy");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);

    delay(500);
  } else if (digitalRead(btnPin) == HIGH && isPressed) {
    isPressed = false;
  }

  if (inicioJogo) {
    playLevel();
  }
}

void playLevel() {
  int sensorsToActivate[4] = {LOW, LOW, LOW, LOW}; // Inicializa sensores como LOW
  for (int i = 0; i < activeSensors; i++) {
    int opcao;
    do {
      opcao = random(1, 5); // Escolhe um sensor aleatório para ativar
    } while (sensorsToActivate[opcao - 1] == HIGH); // Garante que a ativação seja única
    sensorsToActivate[opcao - 1] = HIGH;
  }

  digitalWrite(led1, sensorsToActivate[0]);
  digitalWrite(led2, sensorsToActivate[1]);
  digitalWrite(led3, sensorsToActivate[2]);
  digitalWrite(led4, sensorsToActivate[3]);

  bool sensoresAcertados[4] = {false, false, false, false}; // Array para rastrear quais sensores foram ativados corretamente
  bool acertou = false;

  while (!acertou) {
    if (sensorsToActivate[0] && ultra1.read() <= 15) {
      sensoresAcertados[0] = true;
      digitalWrite(led1, LOW);
    }
    if (sensorsToActivate[1] && ultra2.read() <= 15) {
      sensoresAcertados[1] = true;
      digitalWrite(led2, LOW);
    }
    if (sensorsToActivate[2] && ultra3.read() <= 15) {
      sensoresAcertados[2] = true;
      digitalWrite(led3, LOW);
    }
    if (sensorsToActivate[3] && ultra4.read() <= 15) {
      sensoresAcertados[3] = true;
      digitalWrite(led4, LOW);
    }

    if (sensorsActivatedCorrectly(sensorsToActivate, sensoresAcertados)) {
      acertou = true;
      processActivation();
    }

    if (millis() - inicio >= tempo) {
      endGame();
      return;
    }
  }

  checkLevelUp();

  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

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
  int pointsGained = 10;

  if ((currentActivationTime - lastActivationTime <= bonusWindow) || (level == 4)) {
    pointsGained += 10;
  }

  score += pointsGained;
  inicio = millis();

  lastActivationTime = currentActivationTime;
}

void checkLevelUp() {
  if (score >= scoreToNextLevel) {
    if (level == 1) {
      levelUp("Medium", 2, 2000, 2, 800);
    } else if (level == 2) {
      levelUp("Hard", 3, 1500, 3, 1200);
    } else if (level == 3) {
      levelUp("NERVOSO", 4, 800, 3, -1);
    }

  }
}

void levelUp(String levelName, int newLevel, unsigned long newTempo, int newActiveSensors, int nextScoreThreshold) {
  inicio = millis();

  level = newLevel;
  tempo = newTempo;
  activeSensors = newActiveSensors;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(levelName);
  tone(buzzerPin, 1000);
  delay(500);
  noTone(buzzerPin);

  if (nextScoreThreshold > 0) {
    scoreToNextLevel = nextScoreThreshold;
  }

  inicio = millis();
}

void endGame() {
  inicioJogo = false;
  noTone(buzzerPin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score Final:");
  lcd.print(score);
  tone(buzzerPin, 1000);

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

  noTone(buzzerPin);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);

  delay(2000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Seja Rapido.");
  lcd.setCursor(0, 1);
  lcd.print("Seja NERVY!");
}
