/****************************************************************************************************************************
  WIOT_ConfigOnSwitch.ino
  For WIO Terminal boards

  WIO_WiFiManager is a library for the SeeedStudiocWIO Terminal/Arduino platform

  Modified from
  1) Tzapu        https://github.com/tzapu/WiFiManager
  2) Ken Taylor   https://github.com/kentaylor
  3) Khoi Hoang   https://github.com/khoih-prog/ESP_WiFiManager

  Built by Khoi Hoang https://github.com/khoih-prog/WIOTerminal_WiFiManager
  Licensed under MIT license
 *****************************************************************************************************************************/

#if !defined(WIO_TERMINAL)
  #error This code is intended to run on the WIO Terminal SAMD51 platform! Please check your Tools->Board setting.
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIO_WIFIMGR_LOGLEVEL_    4

#include <rpcWiFi.h>

#include <DNSServer.h>
#include <WebServer.h>


// Include EEPROM-like API for FlashStorage
#include <FlashAsEEPROM_SAMD.h>             //https://github.com/khoih-prog/FlashStorage_SAMD

#ifndef EEPROM_START
  #define EEPROM_START     0
#endif

/*
    Trigger for inititating config mode is Pin WIO_KEY_C on WIO Terminal
*/
const int TRIGGER_PIN = WIO_KEY_C;
/*
   Alternative trigger pin. Needs to be connected to a button to use this pin. It must be a momentary connection
   not connected permanently to ground. Either trigger pin will work.
*/
const int TRIGGER_PIN2 = WIO_KEY_B;

// SSID and PW for Config Portal
String ssid = "WIOTerminal";
const char* password = "WIOTerminal_Pass";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

// From v1.1.0
// You only need to format the filesystem once
//#define FORMAT_FILESYSTEM       true
#define FORMAT_FILESYSTEM         false

#define MIN_AP_PASSWORD_SIZE    8

#define SSID_MAX_LEN            32
//From v1.0.10, WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN            64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

typedef struct
{
  String wifi_ssid;
  String wifi_pw;
}  WiFi_Credentials_String;

typedef struct
{
  WiFi_Credentials  WiFi_Creds;
} WM_Config;

WM_Config         WM_config;

//////

#define LED_ON      HIGH
#define LED_OFF     LOW


// Indicates whether ESP has WiFi credentials saved from previous session, or double reset detected
bool initialConfig = false;
//////

// Use false if you don't like to display Available Pages in Information Page of Config Portal
// Comment out or use true to display Available Pages in Information Page of Config Portal
// Must be placed before #include <WIO_WiFiManager.h>
#define USE_AVAILABLE_PAGES     false

// New in v1.0.11
#define USING_CORS_FEATURE          true
//////

// Use DHCP
#warning Using DHCP IP
IPAddress stationIP   = IPAddress(0, 0, 0, 0);
IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
IPAddress netMask     = IPAddress(255, 255, 255, 0);

#define USING_ONE_WIFI                    true          

#include <WIOTerminal_WiFiManager.h>              //https://github.com/khoih-prog/WIO_WiFiManager

void heartBeatPrint(void)
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("H");        // H means connected to WiFi
  else
    Serial.print("F");        // F means not connected to WiFi

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(" ");
  }
}

void check_status()
{
  static ulong checkstatus_timeout = 0;

#define HEARTBEAT_INTERVAL    60000L
  // Print hearbeat every HEARTBEAT_INTERVAL (60) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}

// The SAMD FlashStorage will be erased after reflash the new firmware / sketch.
// You're required to enter Config Portal if no WiFi Credentials are stored.
bool loadConfigData(void)
{
  uint16_t offset = EEPROM_START;

  uint8_t* _pointer = (uint8_t *) &WM_config;

  int checkSum = 0;
  int readCheckSum;

  for (int i = 0; i < sizeof(WM_config); i++, _pointer++, offset++)
  {
    *_pointer = EEPROM.read(offset);
    checkSum += *_pointer;
  }

  _pointer = (uint8_t *) &readCheckSum;
      
  for (int i = 0; i < sizeof(readCheckSum); i++, _pointer++, offset++)
  {                  
    *_pointer = EEPROM.read(offset);
  }

  Serial.print(F("Calc. CheckSum = 0x"));
  Serial.print(String(checkSum, HEX));
  Serial.print(F(", Read CheckSum = 0x"));
  Serial.println(String(readCheckSum, HEX));
       
  return (checkSum == readCheckSum);
}

void saveConfigData(void)
{
  uint16_t offset = EEPROM_START;

  uint8_t* _pointer = (uint8_t *) &WM_config;

  int checkSum = 0;

  for (int i = 0; i < sizeof(WM_config); i++, _pointer++, offset++)
  {
    checkSum += *_pointer; 
    EEPROM.write(offset, *_pointer);
  }

  _pointer = (uint8_t *) &checkSum;
      
  for (int i = 0; i < sizeof(checkSum); i++, _pointer++, offset++)
  {              
    EEPROM.write(offset, *_pointer);
  }
      
  EEPROM.commit();

  Serial.print(F("CheckSum = 0x"));
  Serial.println(String(checkSum, HEX));
}

uint8_t connectWiFi(void)
{
  #define WIFI_CONNECT_TIMEOUT        30000L
  #define WHILE_LOOP_DELAY            200L
  #define WHILE_LOOP_STEPS            (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))

  unsigned long startedWiFiAt = millis();

  while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedWiFiAt < WIFI_CONNECT_TIMEOUT ) )
  {
    WiFi.mode(WIFI_STA);
    WiFi.persistent (true);

    // We start by connecting to a WiFi network

    Serial.print("Connecting to ");
    Serial.println(Router_SSID);

    WiFi.begin(Router_SSID.c_str(), Router_Pass.c_str());

    int i = 0;
    while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
    {
      delay(WHILE_LOOP_DELAY);
    }
  }

  return WiFi.status();
}

void processDataFromCP(WIO_WiFiManager &WIO_wifiManager)
{
  // Stored  for later usage, from v1.1.0, but clear first
  memset(&WM_config, 0, sizeof(WM_config));

  String tempSSID = WIO_wifiManager.getSSID();
  String tempPW   = WIO_wifiManager.getPW();

  if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds.wifi_ssid) - 1)
    strcpy(WM_config.WiFi_Creds.wifi_ssid, tempSSID.c_str());
  else
    strncpy(WM_config.WiFi_Creds.wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds.wifi_ssid) - 1);

  if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds.wifi_pw) - 1)
    strcpy(WM_config.WiFi_Creds.wifi_pw, tempPW.c_str());
  else
    strncpy(WM_config.WiFi_Creds.wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds.wifi_pw) - 1);
}

void setup()
{
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_PIN2, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting WIOT_ConfigOnSwitch_1WiFi on " + String(BOARD_NAME));
  Serial.println(WIO_TERMINAL_WIFIMANAGER_VERSION);

  unsigned long startedAt = millis();

  //Local intialization. Once its business is done, there is no need to keep it around
  WIO_WiFiManager WIO_wifiManager;

  WIO_wifiManager.setDebugOutput(true);

  // Use only to erase stored WiFi Credentials
  //WIO_wifiManager.resetSettings();

  //set custom ip for portal
  //WIO_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

  WIO_wifiManager.setMinimumSignalQuality(-1);

  // From v1.0.10 only
  // Set config portal channel, default = 1. Use 0 => random channel from 1-13
  //WIO_wifiManager.setConfigPortalChannel(0);
  //////

  // New from v1.1.1
#if USING_CORS_FEATURE
  WIO_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

  // We can't use WiFi.SSID() in ESP32 as it's only valid after connected.
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS for this purpose
  Router_SSID = WIO_wifiManager.WiFi_SSID();
  Router_Pass = WIO_wifiManager.WiFi_Pass();

  // If no stored WiFi data, load from saved EEPROM 
  if ( (Router_SSID == "") || (Router_Pass == "") )
  {
    // Load stored data and verify CheckSum
    if (loadConfigData())
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds.wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds.wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        WIO_LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds.wifi_ssid, F(", PW = "), WM_config.WiFi_Creds.wifi_pw );
        Router_SSID = WM_config.WiFi_Creds.wifi_ssid;
        Router_Pass = WM_config.WiFi_Creds.wifi_pw;
      }
  
      if ( WiFi.status() != WL_CONNECTED )
      {
        Serial.println(F("ConnectWiFi in setup"));
  
        if ( connectWiFi() == WL_CONNECTED )
        {
          Serial.println("Connected to " + Router_SSID);
          return;
        }
      }
    }
  }

  //Remove this line if you do not want to see WiFi password printed
  Serial.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  // From v1.1.0, Don't permit NULL password
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    WIO_LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);

    Router_SSID = WM_config.WiFi_Creds.wifi_ssid;
    Router_Pass = WM_config.WiFi_Creds.wifi_pw;

    WIO_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
    Serial.println(F("Got stored Credentials. Timeout 120s for Config Portal"));
  }
  else
  {
    Serial.println(F("Open Config Portal without Timeout: No stored Credentials."));
    digitalWrite(PIN_LED, LED_ON); // Turn led on as we are in configuration mode.

    initialConfig = true;
  }

  startedAt = millis();
  
  if (initialConfig)
  {
    Serial.println(F("Starting Config. Portal."));
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    //sets timeout in seconds until configuration portal gets turned off.
    //If not specified device will remain in configuration mode until
    //switched off via webserver or device is restarted.
    //WIO_wifiManager.setConfigPortalTimeout(600);

    // Starts an access point
    if (!WIO_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
      Serial.println(F("Not connected to WiFi but continuing anyway."));
    else
    {
      Serial.println(F("WiFi connected...yeey :)"));
    }

    processDataFromCP(WIO_wifiManager);
    saveConfigData();
  }

  digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.

  Serial.print(F("After "));
  Serial.print((float) (millis() - startedAt) / 1000L);
  Serial.print(F(" secs waiting in setup(), result is "));

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print(F("connected. Local IP: "));
    Serial.println(WiFi.localIP());
  }
  else
    Serial.println(WIO_wifiManager.getStatus(WiFi.status()));
}

void loop()
{
  // is configuration portal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
  {
    Serial.println(F("\nConfig. portal requested."));
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    //Local intialization. Once its business is done, there is no need to keep it around
    WIO_WiFiManager WIO_wifiManager;

    WIO_wifiManager.setMinimumSignalQuality(-1);

    // From v1.0.10 only
    // Set config portal channel, default = 1. Use 0 => random channel from 1-13
    //WIO_wifiManager.setConfigPortalChannel(0);
    //////

    // New from v1.1.1
#if USING_CORS_FEATURE
    WIO_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

    //Check if there is stored WiFi router/password credentials.
    //If not found, device will remain in configuration mode until switched off via webserver.
    Serial.print("Opening configuration portal. ");
    Router_SSID = WIO_wifiManager.WiFi_SSID();
    Router_Pass = WIO_wifiManager.WiFi_Pass();

    // From v1.1.0, Don't permit NULL password
    if ( (Router_SSID != "") && (Router_Pass != "") )
    {
      WIO_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
      Serial.println(F("Got stored Credentials. Timeout 120s"));
    }
    else
      Serial.println(F("No stored Credentials. No timeout"));

    //Starts an access point
    //and goes into a blocking loop awaiting configuration
    if (!WIO_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
    {
      Serial.println(F("Not connected to WiFi but continuing anyway."));
    }
    else
    {
      //if you get here you have connected to the WiFi
      Serial.println(F("connected...yeey :)"));
      Serial.print(F("Local IP: "));
      Serial.println(WiFi.localIP());
    }

    // Only clear then save data if CP entered and with new valid Credentials
    // No CP => stored getSSID() = ""
    if ( String(WIO_wifiManager.getSSID(0)) != "" && String(WIO_wifiManager.getSSID(1)) != "" )
    {
      processDataFromCP(WIO_wifiManager);
      saveConfigData();
    }

    digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.
  }

  // put your main code here, to run repeatedly
  check_status();

}
