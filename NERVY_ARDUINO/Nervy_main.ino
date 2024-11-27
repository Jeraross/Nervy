#include <LiquidCrystal_I2C.h>
#include "Ultrasonic.h"

LiquidCrystal_I2C lcd(0x27, 20, 4); // Verifique se o endereço I2C do seu módulo é 0x27 ou 0x3F

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
const int btnPin = A0;

bool isPressed = false;
bool inicioJogo = false;

unsigned long inicio;
unsigned long tempo;

int score = 0;
int level = 1;
int initialScoreToNextLevel = 200;
int scoreToNextLevel = initialScoreToNextLevel;
int activeSensors = 1;
int lives = 3;
String currentLevelName = "Easy";  // Variável global para armazenar o nome do nível atual

int countdownMelody[] = {392, 440, 494};  // Notas para a contagem regressiva
int countdownDurations[] = {100, 100, 100};

int endGameMelody[] = {262, 196, 131, 98}; // Melodia de fim de jogo
int endGameDurations[] = {300, 300, 300, 600};

// Notas e durações para as melodias
int loseLifeMelody[] = {523, 392, 330, 262};    // C5, G4, E4, C4 (melodia triste)
int loseLifeDurations[] = {150, 150, 150, 300}; // Duracao de cada nota em ms

int levelUpMelody[] = {262, 330, 392, 523};     // C4, E4, G4, C5 (melodia alegre)
int levelUpDurations[] = {150, 150, 150, 300};  // Duracao de cada nota em ms

void playMelody(int melody[], int durations[], int length) {
  for (int i = 0; i < length; i++) {
    tone(buzzerPin, melody[i], durations[i]);
    delay(durations[i] * 1.3); // Pequeno atraso entre as notas
  }
  noTone(buzzerPin); // Para o buzzer no fim da melodia
}

void blinkLCD(int times) {
  for (int i = 0; i < times; i++) {
    lcd.noBacklight();
    delay(200);
    lcd.backlight();
    delay(200);
  }
}

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 1);
  lcd.print("    Seja Rapido.    ");
  lcd.setCursor(0, 2);
  lcd.print("    Seja NERVY!     ");

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
    tempo = 2000;
    score = 0;
    level = 1;
    activeSensors = 1;
    lives = 3;
    scoreToNextLevel = initialScoreToNextLevel;
    currentLevelName = "Easy";  // Define o nome do nível inicial

    lcd.clear();

    for (int i = 1; i <= 3; i++) {
      lcd.setCursor(0, 0);
      lcd.print(i);
      tone(buzzerPin, 1000);
      for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
      playMelody(countdownMelody, countdownDurations, 3);
      delay(500);
      noTone(buzzerPin);
      for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
      delay(500);
    }
    
    lcd.setCursor(0, 0);
    lcd.print("NERVY!");
    
    tone(buzzerPin, 1000);
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
      delay(50);
      for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
      delay(50);
    }
    noTone(buzzerPin);

    inicio = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level: ");
    lcd.print(currentLevelName);  // Exibe o nome do nível inicial
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    lcd.setCursor(0, 2);
    lcd.print("Vidas: ");
    lcd.print(lives);

    delay(500);
  } else if (digitalRead(btnPin) == HIGH && isPressed) {
    isPressed = false;
  }

  if (inicioJogo) {
    playLevel();
  }
}

void playLevel() {
  int sensorsToActivate[4] = {LOW, LOW, LOW, LOW};
  if (level == 4) {
      int randomChance = random(1, 101); 
      
      if (randomChance <= 30) {          
          activeSensors = 1;
      } else if (randomChance <= 60) {   
          activeSensors = 2;
      } else if (randomChance <= 90) {   
          activeSensors = 3;
      } else {                           
          activeSensors = 4;
      }
  }

  int cont = activeSensors;
  for (int i = 0; i < activeSensors; i++) {
    int opcao;
    do {
      opcao = random(1, 5);
    } while (sensorsToActivate[opcao - 1] == HIGH);
    sensorsToActivate[opcao - 1] = HIGH;
  }

  digitalWrite(led1, sensorsToActivate[0]);
  digitalWrite(led2, sensorsToActivate[1]);
  digitalWrite(led3, sensorsToActivate[2]);
  digitalWrite(led4, sensorsToActivate[3]);

  bool sensoresAcertados[4] = {false, false, false, false};
  bool acertou = false;

  while (!acertou) {

    if (ultra1.read() <= 15) {
        if (sensorsToActivate[0]) {
            if (sensoresAcertados[0] == false) {
              cont -= 1;
            }
            sensoresAcertados[0] = true;
            digitalWrite(led1, LOW);
        } else {
            blinkLCD(1);
            loseLife();
            if (lives <= 0) {
              for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
              endGame();
              return;
          }
        }
        if (cont == 0) {
          delay(200); 
        }
    }

    if (ultra2.read() <= 15) {
        if (sensorsToActivate[1]) {
            if (sensoresAcertados[1] == false) {
              cont -= 1;
            }
            sensoresAcertados[1] = true;
            digitalWrite(led2, LOW);
        } else {
            blinkLCD(1);
            loseLife();
            if (lives <= 0) {
              for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
              endGame();
              return;
          }
        }
        if (cont == 0) {
          delay(200); 
        }
    }

    if (ultra3.read() <= 15) {
        if (sensorsToActivate[2]) {
            if (sensoresAcertados[2] == false) {
              cont -= 1;
            }
            sensoresAcertados[2] = true;
            digitalWrite(led3, LOW);
        } else {
            blinkLCD(1);
            loseLife();
            if (lives <= 0) {
              for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
              endGame();
              return;
          }
        }
        if (cont == 0) {
          delay(200); 
        }
    }

    if (ultra4.read() <= 15) {
        if (sensorsToActivate[3]) {
            if (sensoresAcertados[3] == false) {
              cont -= 1;
            }
            sensoresAcertados[3] = true;
            digitalWrite(led4, LOW);
        } else {
            blinkLCD(1);
            loseLife();
            if (lives <= 0) {
              for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
              endGame();
              return;
          }
        }
        if (cont == 0) {
          delay(200); 
        }
    }
  

    if (sensorsActivatedCorrectly(sensorsToActivate, sensoresAcertados)) {
        acertou = true;
        processActivation();
    }

    if ((millis() - inicio >= tempo)) {
        blinkLCD(1);
        loseLife();
        if (lives <= 0) {
          for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
          endGame();
          return;
      }
    }
  }

  checkLevelUp();

  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(currentLevelName);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.setCursor(0, 2);
  lcd.print("Vidas: ");
  lcd.print(lives);
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
  int pointsGained = 10;
  if (level == 4) {
    pointsGained += 10;
  }
  score += pointsGained;
  inicio = millis();
}

void loseLife() {
    lives--;

    // Armazena o estado dos LEDs ativados e desliga-os
    bool activeLEDs[4];
    for (int i = 0; i < 4; i++) {
      activeLEDs[i] = digitalRead(leds[i]);
      digitalWrite(leds[i], LOW);  // Desliga os LEDs ativos
    }

    // Exibe mensagem de destaque e toca melodia de perda de vida
    lcd.clear();
    lcd.setCursor(0, 2);
    lcd.print("    Perdeu Vida!    ");
    playMelody(loseLifeMelody, loseLifeDurations, 4);

    // Pisca a tela LCD para enfatizar a perda de vida
    blinkLCD(3);

    // Restaura o estado dos LEDs para retomar o jogo
    for (int i = 0; i < 4; i++) {
      digitalWrite(leds[i], activeLEDs[i]);  // Liga novamente os LEDs que estavam ativos
    }

    // Atualiza o display com o número de vidas restantes e o nome do nível atual
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level: ");
    lcd.print(currentLevelName);
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    lcd.setCursor(0, 2);
    lcd.print("Vidas: ");
    lcd.print(lives);

    inicio = millis();
}

void checkLevelUp() {
  if (score >= scoreToNextLevel) {
    if (level == 1) {
      levelUp("Medium", 2, 2000, 2, 400);
    } else if (level == 2) {
      levelUp("Hard", 3, 2000, 3, 600);
    } else if (level == 3) {
      levelUp("NERVOSO", 4, 1500, 0, -1);
    }
  }
}

void levelUp(String levelName, int newLevel, unsigned long newTempo, int newActiveSensors, int nextScoreThreshold) {
  level = newLevel;
  tempo = newTempo;
  activeSensors = newActiveSensors;

  currentLevelName = levelName;  // Atualiza o nome do nível atual

  if (level == 4) {
    lives = 1;
  } else {
    lives = 3;  
  }

  lcd.clear();

  if (level == 2) {
    lcd.setCursor(0, 1);
    lcd.print("  Nervos de ferro!  ");
    lcd.setCursor(0, 2);
    lcd.print("   Proximo nivel    ");
  } else if (level == 3) {
    lcd.setCursor(0, 1);
    lcd.print("    Sangue frio!    ");
    lcd.setCursor(0, 2);
    lcd.print("   Avante Nervoso   ");
  } else if (level == 4) {
    lcd.setCursor(0, 1);
    lcd.print("    NERVY TIME!     ");
    lcd.setCursor(0, 2);
    lcd.print("    SEJA RAPIDO     ");
  }

  delay(1500);

  playMelody(levelUpMelody, levelUpDurations, 4);

  delay(1500);

  blinkLCD(3);

  if (nextScoreThreshold > 0) {
    scoreToNextLevel = nextScoreThreshold;
  }
  
  // Atualiza o display com o número de vidas restantes e o nome do nível atual
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(currentLevelName);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.setCursor(0, 2);
  lcd.print("Vidas: ");
  lcd.print(lives);

  inicio = millis();
}

void endGame() {
  inicioJogo = false;

  Serial.print("SCORE:");
  Serial.println(score);
  Serial.println("END");

  noTone(buzzerPin);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("DEU PANE NOS NERVYS!");
  lcd.setCursor(0, 2);
  lcd.print("  Score Final: ");
  lcd.print(score);

  playMelody(endGameMelody, endGameDurations, 4);

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

  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("    Seja Rapido.    ");
  lcd.setCursor(0, 2);
  lcd.print("    Seja NERVY!     ");
}
