#include <Servo.h>
#include <LiquidCrystal.h>

// LCD pin mapping: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Servo entryGate;
Servo exitGate;

// 3-pin ultrasonic sensors
const int entrySensor = 10;
const int exitSensor = 7;

const int servoEntryPin = 6;
const int servoExitPin = 9;

const int threshold = 50;
String slotStatus = "";

void setup() {
  Serial.begin(9600);

  entryGate.attach(servoEntryPin);
  exitGate.attach(servoExitPin);

  pinMode(entrySensor, INPUT); // Initially output to send pulse
  pinMode(exitSensor, INPUT);

  entryGate.write(0); // Gate closed
  exitGate.write(0);  // Gate closed

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Arpit's Parking...");
  delay(1500);
  lcd.clear();
}

long getDistance(int sensorPin) {
  // Send pulse
  pinMode(sensorPin, OUTPUT);
  digitalWrite(sensorPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorPin, LOW);

  // Switch to input and measure
  pinMode(sensorPin, INPUT);
  long duration = pulseIn(sensorPin, HIGH);

  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  // Gate control
  long entryDist = getDistance(entrySensor);
  long exitDist = getDistance(exitSensor);

  Serial.print("Entry: ");
  Serial.print(entryDist);
  Serial.print(" cm | Exit: ");
  Serial.print(exitDist);
  Serial.println(" cm");

  // Entry detection
  if (entryDist > 0 && entryDist < threshold) {
    entryGate.write(90); // Open gate
   // delay(3000);
   // entryGate.write(0);
  } else {
    entryGate.write(0);  // Ensure closed
  }

  // Exit detection
  if (exitDist > 0 && exitDist < threshold) {
    exitGate.write(90); // Open gate
   // delay(3000);
    //exitGate.write(0);

  } else {
    exitGate.write(0);  // Ensure closed
  }

  delay(200);

  // Read serial messages from Arduino 1
  while (Serial.available()) {
    slotStatus = Serial.readStringUntil('\n');
    slotStatus.trim();
    lcd.clear();

    if (slotStatus == "FULL") {
      lcd.setCursor(0, 0);
      lcd.print("Parking Full");
    } 
    else if (slotStatus.startsWith("SLOT:")) {
      String slotList = slotStatus.substring(5); // "1,3,5"
      slotList.replace(",", " ");
      lcd.setCursor(0, 0);
      lcd.print("Free Slots:");
      lcd.setCursor(0, 1);
      lcd.print("S " + slotList);
    }
    else if (slotStatus.startsWith("BILL:")) {
      String billData = slotStatus.substring(5); // "S2,21s,$1"
      int comma1 = billData.indexOf(',');
      int comma2 = billData.lastIndexOf(',');

      String slot = billData.substring(0, comma1);         // S2
      String time = billData.substring(comma1 + 1, comma2); // 21s
      String cost = billData.substring(comma2 + 1);         // $1

      lcd.setCursor(0, 0);
      lcd.print(slot + " Billed");

      lcd.setCursor(0, 1);
      lcd.print(time + " : " + cost);

      delay(4000); // Show billing info for 3 sec
      
    }
    delay(1000);
    lcd.clear();
  }
  
  delay(500);
}
