#define t1 5
#define t2 6
#define t3 7
#define t4 8
#define t5 9
#define t6 10

#define BUZZER_PIN 4
int LED = 13;
int MQ2pin = A0;

long d[6];                   // Distances from sensors
bool slotFree[6];            // Free/occupied status
bool previouslyOccupied[6];  // Track previous state
unsigned long startTime[6];  // Time when a slot became occupied
unsigned long parkedDuration[6];

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
}

long readDistance(int sigPin) {
  pinMode(sigPin, OUTPUT); 
  digitalWrite(sigPin, LOW);
  delayMicroseconds(2);  
  digitalWrite(sigPin, HIGH);
  delayMicroseconds(10);  
  digitalWrite(sigPin, LOW);  
  pinMode(sigPin, INPUT);
  return pulseIn(sigPin, HIGH) / 58.2; // in cm
}

void loop() {
  
 float sensorValue;
  sensorValue = analogRead(MQ2pin);
  
  if(sensorValue >= 250)
  {
  	digitalWrite(LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
     delay(300);
     digitalWrite(BUZZER_PIN, LOW);
     delay(300);
    Serial.print("GasSensorValue: ");
    Serial.print(sensorValue);
    Serial.print("\n");
  }
  else
  {
    digitalWrite(LED, LOW);
    Serial.print("GasSensorValue: ");
    Serial.print(sensorValue);
    Serial.print("\n");
  }
  delay(1000);
  d[0] = readDistance(t1);
  d[1] = readDistance(t2);
  d[2] = readDistance(t3);
  d[3] = readDistance(t4);
  d[4] = readDistance(t5);
  d[5] = readDistance(t6);

  int freeCount = 0;
  for (int i = 0; i < 6; i++) {
    bool isFreeNow = d[i] > 100;

    // Car just parked
    if (!isFreeNow && !previouslyOccupied[i]) {
      startTime[i] = millis();
      previouslyOccupied[i] = true;
    }

    // Car just left
    if (isFreeNow && previouslyOccupied[i]) {
      parkedDuration[i] = millis() - startTime[i];
      previouslyOccupied[i] = false;

      unsigned long seconds = parkedDuration[i] / 1000;
      int cost = seconds / 20;
      if (seconds > 0) {
        Serial.print("BILL:S");
        Serial.print(i + 1);
        Serial.print(",");
        Serial.print(seconds);
        Serial.print("s,$");
        Serial.println(cost);
      }
    }

    slotFree[i] = isFreeNow;
    if (slotFree[i]) freeCount++;
  }

  // Send availability or full signal
  if (freeCount == 0) {
    Serial.println("FULL");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(3000);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    Serial.print("SLOT:");
    for (int i = 0; i < 6; i++) {
      if (slotFree[i]) {
        Serial.print(i + 1);
        if (--freeCount > 0) Serial.print(",");
      }
    }
    Serial.println();
  }

  delay(1000);
}
