// In this code, If PIR is triggered then Laser system will active for 15sec
//And Laser is broked within 15sec then I will receive 2 calls(5sec gap) 
//After 15sec PIR will start detecting again

#include <SoftwareSerial.h>

SoftwareSerial GSM(11, 10);  // TX, RX

char phone_no[] = "+918766494775";

#define PIR 2
#define LDR A0

int ldrThreshold = 500;

bool monitoring = false;
unsigned long startTime = 0;
const unsigned long monitoringTime = 120000; // 15 seconds (Laser will become active for 15sec after PIR is triggered
                                            //If you want active for 1min then update this value to 15000->60000
                                            //120000 means for 2min active
void setup() {

  Serial.begin(9600);
  GSM.begin(9600);

  pinMode(PIR, INPUT);

  Serial.println("Initializing....");
  initModule("AT", "OK", 1000);

  Serial.println("System Ready");
}

void loop() {

  int pirState = digitalRead(PIR);

  // PIR triggered → start monitoring
  if (pirState == HIGH && monitoring == false)
  {
    monitoring = true;
    startTime = millis();
    Serial.println("PIR Triggered - Laser Monitoring for 15 sec");
  }

  // Laser monitoring during 15 sec window
  if (monitoring)
  {
    int ldrValue = analogRead(LDR);

    if (ldrValue < ldrThreshold)
    {
      Serial.println("Laser Broken!");

      callUp(phone_no);
      delay(5000);
      callUp(phone_no);

      monitoring = false;
      Serial.println("Monitoring finished after call");
    }

    // If 15 sec passed without laser break
    if (millis() - startTime >= monitoringTime)
    {
      Serial.println("15 sec finished - No Laser Break");
      monitoring = false;
    }
  }
}

void callUp(char *number) {
  GSM.print("ATD");
  GSM.print(number);
  GSM.println(";");

  delay(30000);

  GSM.println("ATH");
  delay(100);
}

void initModule(String cmd, char *res, int t) {
  while (1) {
    Serial.println(cmd);
    GSM.println(cmd);
    delay(100);

    while (GSM.available() > 0) {
      if (GSM.find(res)) {
        Serial.println(res);
        delay(t);
        return;
      }
    }
    delay(t);
  }
}