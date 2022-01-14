// *-----Library----*

//lib to push noti
#include <Pushsafer.h>

// lib to connect wifi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library


//lib to connect to Servo motor
#include "Servo.h"

// *-----Global variable----*

/*WiFiClientSecure client;*/
WiFiClient client;
ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

/*Servo;*/
//define servo pin
int servo_pin = 13;
Servo myservo;
int angle = 0;

/*Notification;*/
// Pushsafer private or alias key -> to push noti to specific user
#define PushsaferKeyMinh "HQWqjeNKBzvQC48IZjcy"
#define PushsaferKeyPhat "IQoxClj5arO2BzPmFBT0"
#define PushsaferKeyKhiem "uKlZS4vjnvUGu3JbhnU8"
#define PushsaferKeyQuan "kzt18mKyisqNp6cQzP7c"
Pushsafer pushsaferMinh(PushsaferKeyMinh, client);
Pushsafer pushsaferPhat(PushsaferKeyPhat, client);
Pushsafer pushsaferKhiem(PushsaferKeyKhiem, client);
Pushsafer pushsaferQuan(PushsaferKeyQuan, client);

//Initialize a variable to hold false for notification in the first time
// because when start the program the notfication is 0
bool isNotifiedFirstTime = false;


/*Timer;*/
// Set timer to repeat noti if the trash is still full after 15 minutes

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousNotiTime = 0;        // will store last time noti was updated

// constants won't change, 15 minutes = 900 000ms :
//const long interval = 900000;           // interval at which to noti again if the trash is still full (milliseconds)
const long interval = 10000;    // for demo only 10second for each noti

/*Ultra sensor;*/
//set up for ultra sensor Trash
const int trigPinTrash = 12;
const int echoPinTrash = 14;

//set up for ultra sensor People
const int trigPinPeople = 4;
const int echoPinPeople = 5;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034

// initialize duration of sound wave sending back and distance from obj
long duration;
float distanceCm;

// *-----Function----*

/*Trash Ultra sensor;*/
void openTheTrash(){
  // move to 180 degree to get to open position
  angle = 180;     
  myservo.write(angle);                     
  delay(200);
}

void closeTheTrash(){
    // move to 0 degree to get to close position
    angle = 0;       
    myservo.write(angle);
    delay(200);             
}


/*Wifi connection;*/
void wifiConnectionSetUp() {
  wifiMulti.addAP("93E SNA F2", "5678956789");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("CIA CIA", "0933888733"); 
  wifiMulti.addAP("Anonymous_P", "monokuro"); 
  wifiMulti.addAP("Khiem's iPhone", "khongcopass"); 
  wifiMulti.addAP("Quan Vu", "buhbuhlmao1238900010");
//  wifiMulti.addAP("Su Tu Gaming", "conkhigia");
//  wifiMulti.addAP("Paul Pogba 6", "conkhigia");
  wifiMulti.addAP("Connectify-Superidols", "SuperiDamVinhHung");
  wifiMulti.addAP("Yoru", "ilavayou");

  Serial.println("Connecting to Wifi ... ");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(1000);
    Serial.print(".");
  }
}


/*Notification;*/
void pushNoti() {
    pushsaferMinh.debug = true;
    pushsaferPhat.debug = true;
    pushsaferKhiem.debug = true;
    pushsaferQuan.debug = true;
    // Initialize noti obj to push noti

  // Take a look at the Pushsafer.com API at
  // https://www.pushsafer.com/en/pushapi

  struct PushSaferInput notiInput;
  notiInput.message = "The trash is FULL, please PUT it away, it is STINKY!";
  notiInput.title = "TRASH STINK!";
  notiInput.sound = "8";
  notiInput.vibration = "1";
  notiInput.icon = "1";
  notiInput.iconcolor = "#FFCCCC";
  notiInput.priority = "1";
  notiInput.device = "a";
  notiInput.url = "https://www.pushsafer.com";
  notiInput.urlTitle = "Open Pushsafer.com";

  Serial.println(pushsaferMinh.sendEvent(notiInput));
  Serial.println(pushsaferPhat.sendEvent(notiInput));
  Serial.println(pushsaferKhiem.sendEvent(notiInput));
  Serial.println(pushsaferQuan.sendEvent(notiInput));
  Serial.println("Noti sent");

  // client.stop();
}


/*Distance from trash or people;*/
int readingDistanceFrom(int trigPin, int echoPin) {
    // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
 
  return distanceCm;
}

/*Run only once;*/
void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

//Set up pin for ultrasonic for trash distance
  pinMode(trigPinTrash, OUTPUT); // Sets the trigPinTrash as an Output
  pinMode(echoPinTrash, INPUT); // Sets the echoPinTrash as an Input

//Set up pin for ultrasonic for people distance
  pinMode(trigPinPeople, OUTPUT); // Sets the trigPinPeople as an Output
  pinMode(echoPinPeople, INPUT); // Sets the echoPinPeople as an Input

// Set up for servo motor
  myservo.attach(servo_pin);
  myservo.write(0);

// Set up wifi connection
  wifiConnectionSetUp();
}

/*Main program;*/
void loop() {
  // the program will run forever to check the trash emptiness for every 15 minutes
  
  double distanceFromTrash = readingDistanceFrom(trigPinTrash,echoPinTrash);
  Serial.print("Distance (cm) from Trash: ");
  Serial.println(distanceFromTrash);

  // Noti the user if the trash is full
  if (distanceFromTrash <= 7) {
    // Check if this noti is the first time
    if (!isNotifiedFirstTime) {
          pushNoti();
          isNotifiedFirstTime = true;
          previousNotiTime = millis();
          
//          If notified first time already => check the next interval, if the trash is still full => notified again
    } else {
      
//    this variable to store current time
        unsigned long currentNotiTime = millis();

// handle the case when millis() return back to 0 when the program has started for 50 days and count up again
         if (currentNotiTime < previousNotiTime) {
          // reset the previousNotiTime back to 0
          previousNotiTime = 0;
         }

// If pass the interval => noti again
        if (currentNotiTime - previousNotiTime >= interval) {
          // save the last time notification to be current
          previousNotiTime = currentNotiTime;
          // push notification
          pushNoti();
    }
  }
  }

  double distanceFromPeople = readingDistanceFrom(trigPinPeople,echoPinPeople);
  Serial.print("Distance (cm) from People: ");
  Serial.println(distanceFromPeople);

// open or close the trash whenever there is people nearby
  if (distanceFromPeople <= 20) {
    openTheTrash();
  } else {
    closeTheTrash();
  }
}
