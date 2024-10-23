#include "Ultrasonic.h"

Ultrasonic ultra1(6, 7);
Ultrasonic ultra2(8, 9);
Ultrasonic ultra3(10, 11);
Ultrasonic ultra4(12, 13);

int distance;

const int led1 = 5;
const int led2 = 2;
const int led3 = 3;
const int led4 = 4;

const int btnPin = A0;
bool isPressed = false;
bool inicioJogo = false;

int countClick = 0;

int opcao;
bool acertou = false;
bool esgotado = false;

unsigned long inicio;
unsigned long tempo = 5000;

void setup() {
  Serial.begin(9600);

  pinMode(btnPin, INPUT_PULLUP);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  randomSeed(analogRead(A1));
}

void loop() {

  if (digitalRead(btnPin) == LOW && isPressed == false) {
    isPressed = true;
    inicioJogo = true;
    inicio = millis();
    tempo = 5000;

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
    delay(500);

    digitalWrite(led1, LOW);
    digitalWrite(led4, LOW);
    delay(500);

    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    delay(500);
    
  } else if (digitalRead(btnPin) == HIGH && isPressed == true) {
    isPressed = false;
  }

  if (inicioJogo) {
    opcao = random(1, 5);

    if (opcao == 1) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      Serial.println("opcao 1");
    } else if (opcao == 2) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      Serial.println("opcao 2");
    } else if (opcao == 3) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, LOW);
      Serial.println("opcao 3");
    } else if (opcao == 4) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, HIGH);
      Serial.println("opcao 4");
    }

    acertou = false;
    while (!acertou) {
      if (opcao == 1) {
        if (ultra1.read() <= 15) {
          acertou = true;
          inicio = millis();
          //Serial.println("acertou 1");
        }
      } else if (opcao == 2) {
        if (ultra2.read() <= 15) {
          acertou = true;
          inicio = millis();
          //Serial.println("acertou 2");
        }
      } else if (opcao == 3) {
        if (ultra3.read() <= 15) {
          acertou = true;
          inicio = millis();
          //Serial.println("acertou 3");
        }
      } else if (opcao == 4) {
        if (ultra4.read() <= 15) {
          acertou = true;
          inicio = millis();
          //Serial.println("acertou 4");
        }
      }

      if (acertou) {
        //Serial.println("Tempo diminuido");
        tempo -= 50;

        if (tempo < 500) {
          tempo = 500;
        }
      }

      if (millis() - inicio >= tempo) {
        inicioJogo = false;
        // exibir uma mensagem antes do break
        Serial.println(tempo);
        Serial.println("Estourou o tempo");
        for(int i = 0; i<5; i++) {
          digitalWrite(led1, HIGH);
          digitalWrite(led2, HIGH);
          digitalWrite(led3, HIGH);
          digitalWrite(led4, HIGH);
          delay(200);

          digitalWrite(led1, LOW);
          digitalWrite(led2, LOW);
          digitalWrite(led3, LOW);
          digitalWrite(led4, LOW);
          delay(200);
        }
        break;
      }
    }

    //delay(1000);
  }
}