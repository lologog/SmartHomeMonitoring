void setup() 
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 start");
}

void loop() 
{
  Serial.println("test");
  delay(1000);
}