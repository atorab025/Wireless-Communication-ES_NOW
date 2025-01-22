//Slave 


#include <esp_now.h>
#include <WiFi.h>

#define LED_Pin 4
#define BTN_Pin 2
#define SLAVE_BTN1_Pin 15 

int BTN_State;
int SLAVE_BTN1_State;
int LED_State_Send = 0;
int LED_State_Receive;

typedef struct struct_message {
  int led;
  int slaveId;
} struct_send;

struct_send send_Data;
struct_send receive_Data;

bool slaveLED_Blink = false;
unsigned long lastDataTime = 0;
const unsigned long sendInterval = 2000; 

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status to MAC ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print("\tStatus: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receive_DataData, sizeof(receive_Data));
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Slave ID: ");
  Serial.println(receive_Data.slaveId);
  LED_State_Receive = receive_Data.led;
  Serial.print("Receive Data: ");
  Serial.println(LED_State_Receive);
  digitalWrite(LED_Pin, LED_State_Receive);

    SLAVE_BTN1_State = digitalRead(SLAVE_BTN1_Pin);
  if (SLAVE_BTN1_State == 1) {
    digitalWrite(LED_Pin, LOW); 
    slaveLED_Blink = false;
  }

  digitalWrite(LED_Pin, HIGH);
  slaveLED_Blink = true;
  Serial.println("<<<<<");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_Pin, OUTPUT);
  pinMode(BTN_Pin, INPUT);
  pinMode(SLAVE_BTN1_Pin, INPUT);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  uint8_t masterAddress[] = {0xB0, 0xB2, 0x1C, 0xA7, 0xE3, 0xE0}; 
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  unsigned long currentTime = millis();
  BTN_State = digitalRead(BTN_Pin);

  if (BTN_State == 1 && currentTime - lastDataSendTime >= sendDataInterval) {
      if (LED_State_Receive != 0) {
      LED_State_Send = 1;
      send_Data.led = LED_State_Send;
      send_Data.slaveId = 2;

      Serial.println();
      Serial.print(">>>>> Send data to Master");

      esp_err_t result = esp_now_send(NULL, (uint8_t *)&send_Data, sizeof(send_Data));

      if (result != ESP_OK) {
        Serial.println("Sent with success to Master");
      } else {
        Serial.println("Error sending the data to Master");
      }

      lastDataSendTime = currentTime; 
    }

    while (BTN_State == 1) {
      BTN_State = digitalRead(BTN_Pin);
      delay(10);
    }
  }
  
  SLAVE_BTN1_State = digitalRead(SLAVE_BTN1_Pin);
  if (SLAVE_BTN1_State != 1) {
    digitalWrite(LED_Pin, LOW); 
    slaveLED_Blink = false;
        send_Data.led = 0;
    send_Data.slaveId = 2; 
    esp_now_send(NULL, (uint8_t *)&send_Data, sizeof(send_Data));
  }

    if (slaveLED_Blink) {
    digitalWrite(LED_Pin, !digitalRead(LED_Pin));
    delay(500); 
  }
}
