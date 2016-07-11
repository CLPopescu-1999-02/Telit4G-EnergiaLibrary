#define PIN 10

void setup()
{
  Serial.begin(115200);  // start serial for output
  pinMode(PIN, OUTPUT); 
}

void loop()
{
  Serial.println("Setting pin HIGH.");
  digitalWrite(PIN, HIGH);
  sleep(1000);
  
  Serial.println("Setting pin LOW.");
  digitalWrite(PIN, LOW);
  sleep(1000);
}
