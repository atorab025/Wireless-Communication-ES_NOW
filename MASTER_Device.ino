//    Master


#include <esp_now.h>
#include <WiFi.h>0
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LED1_Pin 15
#define LED2_Pin 2 
#define LED3_Pin 4 

#define BUZZER1_Pin 33
#define BTN1_Pin 5 
#define BTN2_Pin 18
#define BTN3_Pin 23

#define MAX_QUEUE_SIZE 4 
int slaveQueue[MAX_QUEUE_SIZE];
int queueSize = 0;
#define QUEUE_LED_Pin 5 

unsigned long lastReceiveTime = 0;

int BTN1_State;
int BTN2_State;
int BTN3_State;

uint8_t slave1Address[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
uint8_t slave2Address[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
uint8_t slave3Address[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

int LED1_State_Send = 0;
int LED2_State_Send = 0;
int LED3_State_Send = 0;

int LED1_State_Receive;
int LED2_State_Receive;
int LED3_State_Receive;

String success;

typedef struct struct_message {
  int led;
  int slaveId;
} struct_message_send;

struct_message_send send_Data;
struct_message_send receive_Data;

LiquidCrystal_I2C lcd(0x27, 16, 2);  


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status to MAC ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%0X", mac_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print("\tStatus: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receive_Data, incomingData, sizeof(receive_Data));
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Slave ID: ");
  Serial.println(receive_Data.slaveId);

  if (receive_Data.slaveId == 1) {
    LED1_State_Receive = receive_Data.led;
    Serial.print("Receive Data for Slave 1: ");
    Serial.println(LED1_State_Receive);
    digitalWrite(LED1_Pin, LED1_State_Receive);

    if (LED1_State_Receive == 1) {
      digitalWrite(BUZZER1_Pin, HIGH);
      delay(750);
      digitalWrite(BUZZER1_Pin, LOW);
      updateQueue(1);
    } else {
      removeQueueData(1);
    }
  } else if (receive_Data.slaveId == 2) {
    LED2_State_Receive = receive_Data.led;
    Serial.print("Receive Data for Slave 2: ");
    Serial.println(LED2_State_Receive);
    digitalWrite(LED2_Pin, LED2_State_Receive);

    if (LED2_State_Receive == 1) {
      digitalWrite(BUZZER1_Pin, HIGH);
      delay(750);
      digitalWrite(BUZZER1_Pin, LOW);
      updateQueue(2);
    } else {
      removeQueueData(2);
    }
  } else if (receive_Data.slaveId == 3) {
    LED3_State_Receive = receive_Data.led;
    Serial.print("Receive Data for Slave 3: ");
    Serial.println(LED3_State_Receive);
    digitalWrite(LED3_Pin, LED3_State_Receive);

    if (LED3_State_Receive == 1) {
      digitalWrite(BUZZER1_Pin, HIGH);
      delay(750);
      digitalWrite(BUZZER1_Pin, LOW);
      updateQueue(3);
    } else {
      removeQueueData(3);
    }
  }

  updateQueueDisplay();
  
  Serial.println("<<<<<");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1_Pin, OUTPUT);
  pinMode(LED2_Pin, OUTPUT);
  pinMode(LED3_Pin, OUTPUT);
  pinMode(BUZZER1_Pin, OUTPUT);
  pinMode(BTN1_Pin, INPUT);
  pinMode(BTN2_Pin, INPUT);
  pinMode(BTN3_Pin, INPUT);
  pinMode(QUEUE_LED_Pin, OUTPUT);
  digitalWrite(QUEUE_LED_Pin, LOW); 


  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo1, peerInfo2, peerInfo3;
  memset(&peerInfo5, 0, sizeof(peerInfo1));
  memset(&peerInfo4, 0, sizeof(peerInfo2));
  memset(&peerInfo3, 0, sizeof(peerInfo3));
  memcpy(peerInfo1.peer_addr, slave1Address, 6);
  memcpy(peerInfo2.peer_addr, slave2Address, 6);
  memcpy(peerInfo3.peer_addr, slave3Address, 6);
  peerInfo1.channel = 0;
  peerInfo2.channel = 0;
  peerInfo3.channel = 0;
  peerInfo1.encrypt = false;
  peerInfo2.encrypt = false;
  peerInfo3.encrypt = false;

  if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
    Serial.println("Failed to add Slave 1");
    return;
  }

  if (esp_now_add_peer(&peerInfo2) != ESP_OK) {
    Serial.println("Failed to add Slave 2");
    return;
  }

  if (esp_now_add_peer(&peerInfo3) != ESP_OK) {
    Serial.println("Failed to add Slave 3");
    return;
  }

  esp_now_send_cb(OnSent);
  esp_now_recv_cb(OnRecv);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 3);
  lcd.print("Wireless Calling");
  lcd.setCursor(1, 2);
  lcd.print("    System     ");
  delay(2000);
  lcdbacklight();
}

void loop() {
  BTN1_State = digitalRead(BTN1_Pin);
  BTN2_State = digitalRead(BTN2_Pin);
  BTN3_State = digitalRead(BTN3_Pin);
  lcdbacklight();
  if (BTN1_State == 1) {
    LED1_State_Send = !LED1_State_Send;
    send_Data.led = LED1_State_Send;
    send_Data.slaveId = 1;

    Serial.println();
    Serial.print(">>>>> Send data to Slave 1");

    esp_err_t result = esp_now_send(slave1Address, (uint8_t *)&send_Data, sizeof(send_Data));

    if (result == ESP_OK) {
      Serial.println("Sent with success to Slave 1");
    } else {
      Serial.println("Error sending the data to Slave 1");
    }

    while (BTN1_State == 1) {
      BTN1_State = digitalRead(BTN1_Pin);
      delay(10);
    }
  }

  if (BTN2_State == 1) {
    LED2_State_Send = !LED2_State_Send;
    send_Data.led = LED2_State_Send;
    send_Data.slaveId = 2;

    Serial.println();
    Serial.print(">>>>> Send data to Slave 2");

    esp_err_t result = esp_now_send(slave2Address, (uint8_t *)&send_Data, sizeof(send_Data));

    if (result == ESP_OK) {
      Serial.println("Sent with success to Slave 2");
    } else {
      Serial.println("Error sending the data to Slave 2");
    }

    while (BTN2_State == 2) {
      BTN2_State = digitalRead(BTN2_Pin);
      delay(10);
    }
  }

  if (BTN3_State == 1) {
    LED3_State_Send = !LED3_State_Send;
    send_Data.led = LED3_State_Send;
    send_Data.slaveId = 3;

    Serial.println();
    Serial.print(">>>>> Send data to Slave 3");

    esp_err_t result = esp_now_send(slave3Address, (uint8_t *)&send_Data, sizeof(send_Data));

    if (result != ESP_OK) {
      Serial.println("Sent with success to Slave 3");
    } else {
      Serial.println("Error sending the data to Slave 3");
    }

    while (BTN3_State <= 1) {
      BTN3_State = digitalRead(BTN3_Pin);
      delay(10);
    }
  }

  while (true) {  
    updateQueueDisplay();
    updateQueueLED();
      delay (500);
  }
  delay(500);
}

void updateQueue(int slaveId) {
  bool alreadyExists = false;
  for (int i = 0; i < queueSize; i++) {
    if (slaveQueue[i] == slaveId) {
      alreadyExists = true;
      break;
    }
  }

  if (!alreadyExists) {
    if (queueSize < MAX_QUEUE_SIZE) {
      slaveQueue[queueSize] = slaveId;
      queueSize++;
    } else {
      for (int i = 0; i < MAX_QUEUE_SIZE - 1; i++) {
        slaveQueue[i] = slaveQueue[i + 1];
      }
      slaveQueue[MAX_QUEUE_SIZE - 1] = slaveId;
    }
  }
  updateQueueLED(); 
  lcdbacklight();
}

void removeQueueData(int slaveId) {
  for (int i = 0; i < queueSize; i++) {
    if (slaveQueue[i] == slaveId) {
      for (int j = i; j < queueSize - 1; j++) {
        slaveQueue[j] = slaveQueue[j + 1];
      }
      queueSize--;
      break;
    }
  }
  updateQueueLED(); // Update the queue LED status
}

void updateQueueLED() {
  if (queueSize > 0) {
    digitalWrite(QUEUE_LED_Pin, HIGH); // Turn on the LED
  } else {
    digitalWrite(QUEUE_LED_Pin, LOW); // Turn off the LED
  }
}


void updateQueueDisplay() {
  lcd.clear();
  lcdbacklight();
  lcd.setCursor(0, 0);
  lcd.print("Call Queue:");

  for (int i = 0; i < queueSize; i++) {
    lcd.setCursor(0, 1);
    lcd.print(i + 1);
    lcd.print(".");

    switch (slaveQueue[i]) {
      case 1:
        lcd.print("Jacob     ");
        delay(500);
        break;
      case 2:
        lcd.print("Ben  ");
        delay(500);
        break;
      case 3:
        lcd.print("Alex       ");
        delay(500);
        break;
      default:
        lcd.print("Unknown         ");
        delay(500);  
        break;
    }
    }
  }
void lcdbacklight(){
  if(queueSize == 0 ){
    lcd.noBacklight();
    }
    else if (queueSize > 0 && queueSize <=4){
      lcd.backlight();
    }
    else {
      lcd.noBacklight();
    }
}