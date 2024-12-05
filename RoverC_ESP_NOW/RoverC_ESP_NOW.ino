//StickC  "EAC"  works as is. F/B, L/R
//usbserial-95522B3EAC
//MAC: 94:b9:7e:8c:b9:7c
//as RoverC
#include <Wire.h>
#include <M5StickCPlus.h>
#include <esp_now.h>
#include <WiFi.h>
#include "RoverC.h"
//Address of OTHER unit (JoyC)
//MAC: 94:b9:7e:8c:7c:e8
uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0x8C, 0x7C, 0xE8};
String success;

typedef struct struct_message {
    int Left_Right;
    int Fwd_Back;
    int Speed;
    int Yaw;
} struct_message;
struct_message fromJoyC;

//declared variables
int fwd;
int back;
int left;
int right;
int turn_left;
int turn_right;
int angle;
esp_now_peer_info_t peerInfo;

void setup() {
    M5.begin();

    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    // Register peer
    
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);

    M5.Axp.ScreenBreath(15);
    M5.Lcd.setRotation(0);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(5, 0);  //horiz, vert
    M5.Lcd.print("F/B=");
    M5.Lcd.setCursor(5, 30);  //horiz, vert
    M5.Lcd.print("L/R=");
    M5.Lcd.setCursor(5, 60);  //horiz, vert
    M5.Lcd.print("YAW=");
    M5.Lcd.setCursor(5, 90);  //horiz, vert
    M5.Lcd.print("angle=");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(5, 130);  //horiz, vert
    M5.Lcd.print("FWD");
    M5.Lcd.setCursor(5, 160);  //horiz, vert
    M5.Lcd.print("BACK");

    M5.Lcd.setCursor(5, 190);  //horiz, vert
    M5.Lcd.print("LEFT");
    M5.Lcd.setCursor(5, 220);  //horiz, vert
    M5.Lcd.print("RIGHT");

    RoverC_Init();
    Move_stop(100);
    Servo_angle(2, 45);

}

void loop() {
    M5.update();
    MapDisplay();
    movement();
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
    memcpy(&fromJoyC, incomingData, sizeof(fromJoyC));
}

//  L0  100C  200R

void movement() {

    left = 0;
    right = 0;

    if (fromJoyC.Fwd_Back > 110) {
        fwd = map(fromJoyC.Fwd_Back, 110, 200, 0, 100);
    }
    else {
        fwd = (0);
    }
    Move_forward(fwd);

    if (fromJoyC.Fwd_Back < 90) {
        back = map(fromJoyC.Fwd_Back, 90, 10, 0, 100);
    }
    else {
        back = 0;
    }
    Move_back(back);



    if (fromJoyC.Left_Right < 90) {
        turn_left = map(fromJoyC.Left_Right, 90, 10, 0, 100);
    }
    else {
        turn_left = (0);
    }
    Move_turnleft(turn_left);



    if (fromJoyC.Left_Right > 110) {
        turn_right = map(fromJoyC.Left_Right, 110, 200, 0, 100);
    }
    else {
        turn_right = (0);
    }
    Move_turnright(turn_right);



    //Servo sweep
    angle = map(fromJoyC.Yaw, 200, 0, 0, 90);
    Servo_angle(2, angle);


    //void Move_forward(int8_t Speed);   EXAMPLEs
    //void Move_turnleft(int8_t Speed);
    //void Move_left(int8_t Speed);

}


void MapDisplay() {
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(80, 0);
    M5.Lcd.fillRect(80, 0, 100, 20, BLACK);
    M5.Lcd.print(fromJoyC.Fwd_Back);

    M5.Lcd.setCursor(80, 30);
    M5.Lcd.fillRect(80, 30, 100, 20, BLACK);
    M5.Lcd.print(fromJoyC.Left_Right);

M5.Lcd.setCursor(80, 60);
    M5.Lcd.fillRect(80, 60, 100, 20, BLACK);
    M5.Lcd.print(fromJoyC.Yaw);

    M5.Lcd.setCursor(80, 90);
    M5.Lcd.fillRect(80, 90, 100, 20, BLACK);
    M5.Lcd.print(angle);

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(80, 130);
    M5.Lcd.fillRect(80, 130, 100, 20, BLACK);
    M5.Lcd.print(fwd);

    M5.Lcd.setCursor(80, 160);
    M5.Lcd.fillRect(80, 160, 100, 20, BLACK);
    M5.Lcd.print(back);

    M5.Lcd.setCursor(80, 190);
    M5.Lcd.fillRect(80, 190, 100, 20, BLACK);
    M5.Lcd.print(turn_left);

    M5.Lcd.setCursor(80, 220);
    M5.Lcd.fillRect(80, 220, 100, 20, BLACK);
    M5.Lcd.print(turn_right);


}
