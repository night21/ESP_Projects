int a1a = 12;
int a1b = 14;
int b1a = 15;
int b1b = 13;

int i = 560;

void setup() {
  Serial.begin(9600); 
  delay(100);
  pinMode(a1a, OUTPUT);
  pinMode(a1b, OUTPUT);
  pinMode(b1a, OUTPUT);
  pinMode(b1b, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.println(i);
      analogWrite(a1b,i);
      analogWrite(b1b,i);
      digitalWrite(a1a,LOW);
      digitalWrite(b1a,LOW);
      delay(1000);
      
}
