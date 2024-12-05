//MAC: 94:b9:7e:8c:7c:e8 My StickCPlus for the 'Wand'
//as Joy
#include <M5StickCPlus.h>
#include <esp_now.h>
#include <WiFi.h>

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

//Address of OTHER unit

uint8_t broadcastAddress[] = { 0x94, 0xB9, 0x7E, 0x8C, 0xB9, 0x7C };  //MAC of the 'Receiver' RoverC

String success;


typedef struct struct_message {
  int Left_Right;
  int Fwd_Back;
  int Speed;
  int Yaw;
} struct_message;
struct_message toBugC;
esp_now_peer_info_t peerInfo;



void setup() {
  M5.begin();
  Wire.begin(0, 26, 400000);
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Core Failed to add peer");
    return;
  }

  M5.Imu.Init();

  M5.Lcd.setRotation(0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, 5);  //horiz, vert
  M5.Lcd.print("F/B=");
  M5.Lcd.setCursor(5, 35);  //horiz, vert
  M5.Lcd.print("L/R=");

  M5.Axp.ScreenBreath(15);
}


void loop() {
  M5.update();

  M5.IMU.getAccelData(&accX, &accY, &accZ);
  int cmdFwd_Back = accY * 100;
  int cmdLeft_Right = accX * 100;

  toBugC.Fwd_Back = map(cmdFwd_Back, 100, -100, 0, 200);  //  F/R  (RH Joystick)

  toBugC.Left_Right = map(cmdLeft_Right, -100, 100, 0, 200);  //  L/R  (RH Joystick)

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&toBugC, sizeof(toBugC));
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(80, 5);
  M5.Lcd.fillRect(80, 5, 100, 20, BLACK);
  M5.Lcd.print(toBugC.Fwd_Back);

  M5.Lcd.setCursor(80, 35);
  M5.Lcd.fillRect(80, 35, 100, 20, BLACK);
  M5.Lcd.print(toBugC.Left_Right);

  delay(100);
}


// OnDataSent when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
}
