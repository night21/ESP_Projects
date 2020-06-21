
void setup() {
  Serial.begin(9600);
  pinMode(D1, OUTPUT);
  pinMode(D2, INPUT);
  Serial.println("Setup complete");
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("Loop started");
  unsigned long duration, distance;
  digitalWrite(D1, LOW);
  delayMicroseconds(2);                      // 10uS trigger
  digitalWrite(D1, HIGH);
  delayMicroseconds(10);                      // 10uS trigger
  digitalWrite(D1, LOW);
  duration = pulseInLong(D2, HIGH, 12077);
  distance = (duration/2) *331.2/10000;
  if (duration != 0) {
  Serial.print(duration);
  Serial.println(" ms");
  Serial.print(distance);
  Serial.println(" cm");
  } else {
    Serial.println("No value");
  }
  
  delay(5000);
}
