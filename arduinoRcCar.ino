#include <DIYables_IRcontroller.h>  
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10,11);

#define IR_RECEIVER_PIN 9
#define ENA_PIN 7
#define IN1_PIN 6
#define IN2_PIN 5
#define IN3_PIN 4
#define IN4_PIN 3
#define ENB_PIN 2

DIYables_IRcontroller_17 irController(IR_RECEIVER_PIN, 200);

void setup() {
  Serial.begin(9600);
  irController.begin();

  BTSerial.begin(9600);   // HM-10 default AT mode baud rate

  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);

  digitalWrite(ENA_PIN, HIGH);
  digitalWrite(ENB_PIN, HIGH);
}


void loop() {
  if (BTSerial.available()) {
    char c = BTSerial.read();
    Serial.write(c);   // echo to Serial monitor

    if(c == '0') {
       Serial.println("MOVING FORWARD");
       CAR_moveForward();
    }
    else if(c == '1') {
      Serial.println("MOVING BACKWARD");
      CAR_moveBackward();
    }
    else if(c == '2') {
      Serial.println("TURNING LEFT");
      CAR_turnLeft();
    }
    else if(c == '3') {
      Serial.println("TURNING RIGHT");
      CAR_turnRight();
    }
    else {
      Serial.println("STOP");
      CAR_stop();
    }
  }
}


void CAR_moveForward() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}

void CAR_moveBackward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
}

void CAR_turnLeft() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}

void CAR_turnRight() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}

void CAR_stop() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}
