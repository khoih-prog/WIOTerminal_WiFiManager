/****************************************************************************************************************************
  ConfigOnStartup.ino
  For WIO Terminal boards

  WIO_WiFiManager is a library for the SeeedStudiocWIO Terminal/Arduino platform

  Modified from
  1) Tzapu        https://github.com/tzapu/WiFiManager
  2) Ken Taylor   https://github.com/kentaylor
  3) Khoi Hoang   https://github.com/khoih-prog/ESP_WiFiManager

  Built by Khoi Hoang https://github.com/khoih-prog/WIOTerminal_WiFiManager
  Licensed under MIT license
 *****************************************************************************************************************************/
/****************************************************************************************************************************
   This example will open a configuration portal for 60 seconds when first powered up if the boards has stored WiFi Credentials.
   Otherwise, it'll stay indefinitely in ConfigPortal until getting WiFi Credentials and connecting to WiFi

   WIO_ConfigOnStartup is a a bettter example for most situations but this has the advantage
   that no pins or buttons are required on the WIO_Terminal device at the cost of delaying
   the user sketch for the configurable period (CONFIG_PORTAL_TIMEOUT_SEC) that the configuration portal is open.

   Also in this example a password is required to connect to the configuration portal
   network. This is inconvenient but means that only those who know the password or those
   already connected to the target WiFi network can access the configuration portal and
   the WiFi network credentials will be sent from the browser over an encrypted connection and
   can not be read by observers.
 *****************************************************************************************************************************/

#if !defined(WIO_TERMINAL)
  #error This code is intended to run on the WIO Terminal SAMD51 platform! Please check your Tools->Board setting.
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIO_WIFIMGR_LOGLEVEL_    4

#include <rpcWiFi.h>

#include <WiFiMulti.h>
WiFiMulti wifiMulti;

#include <DNSServer.h>
#include <WebServer.h>

// Include EEPROM-like API for FlashStorage
#include <FlashAsEEPROM_SAMD.h>             //https://github.com/khoih-prog/FlashStorage_SAMD

#ifndef EEPROM_START
  #define EEPROM_START     0
#endif

// SSID and PW for Config Portal
String ssid = "WIOTerminal";
const char* password = "WIOTerminal_Pass";

#define CONFIG_PORTAL_TIMEOUT_SEC       120

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

#define LED_ON      HIGH
#define LED_OFF     LOW

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

#define NUM_WIFI_CREDENTIALS      2

typedef struct
{
  WiFi_Credentials  WiFi_Creds [NUM_WIFI_CREDENTIALS];
} WM_Config;

WM_Config         WM_config;

//////

// Indicates whether ESP has WiFi credentials saved from previous session, or double reset detected
bool initialConfig = false;
//////

// Use false if you don't like to display Available Pages in Information Page of Config Portal
// Comment out or use true to display Available Pages in Information Page of Config Portal
// Must be placed before #include <WIO_WiFiManager.h>
#define USE_AVAILABLE_PAGES     false

// New in v1.0.11
#define USING_CORS_FEATURE          false
//////

// Use DHCP
#warning Using DHCP IP
IPAddress stationIP   = IPAddress(0, 0, 0, 0);
IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
IPAddress netMask     = IPAddress(255, 255, 255, 0);

#include <WIOTerminal_WiFiManager.h>              //https://github.com/khoih-prog/WIO_WiFiManager

// Function Prototypes
uint8_t connectMultiWiFi(void);

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

void check_WiFi(void)
{
  if ( (WiFi.status() != WL_CONNECTED) )
  {
    Serial.println("\nWiFi lost. Call connectMultiWiFi in loop");
    connectMultiWiFi();
  }
}  

void check_status(void)
{
  static ulong checkstatus_timeout  = 0;
  static ulong checkwifi_timeout    = 0;

  static ulong current_millis;

#define WIFICHECK_INTERVAL    1000L
#define HEARTBEAT_INTERVAL    10000L

  current_millis = millis();
  
  // Check WiFi every WIFICHECK_INTERVAL (1) seconds.
  if ((current_millis > checkwifi_timeout) || (checkwifi_timeout == 0))
  {
    check_WiFi();
    checkwifi_timeout = current_millis + WIFICHECK_INTERVAL;
  }

  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((current_millis > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = current_millis + HEARTBEAT_INTERVAL;
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

uint8_t connectMultiWiFi(void)
{
  // This better be 0 to shorten the connect time
  #define WIFI_MULTI_1ST_CONNECT_WAITING_MS       0
  #define WIFI_MULTI_CONNECT_WAITING_MS           100L
  
  uint8_t status;

  WIO_LOGERROR(F("ConnectMultiWiFi with :"));
  
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    WIO_LOGERROR3(F("* Flash-stored Router_SSID = "), Router_SSID, F(", Router_Pass = "), Router_Pass );
  }

  for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {
    // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
    if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
    {
      WIO_LOGERROR3(F("* Additional SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
    }
  }
  
  WIO_LOGERROR(F("Connecting MultiWifi..."));

  //WiFi.mode(WIFI_STA);

  int i = 0;
  status = wifiMulti.run();
  delay(WIFI_MULTI_1ST_CONNECT_WAITING_MS);

  while ( ( i++ < 10 ) && ( status != WL_CONNECTED ) )
  {
    status = wifiMulti.run();

    if ( status == WL_CONNECTED )
      break;
    else
      delay(WIFI_MULTI_CONNECT_WAITING_MS);
  }

  if ( status == WL_CONNECTED )
  {
    WIO_LOGERROR1(F("WiFi connected after time: "), i);
    WIO_LOGERROR3(F("SSID:"), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
    WIO_LOGERROR1(F("IP address:"), WiFi.localIP() );
  }
  else
    WIO_LOGERROR(F("WiFi not connected"));

  return status;
}

void processDataFromCP(WIO_WiFiManager &WIO_wifiManager)
{
  // Stored  for later usage, from v1.1.0, but clear first
  memset(&WM_config, 0, sizeof(WM_config));

  for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {
    String tempSSID = WIO_wifiManager.getSSID(i);
    String tempPW   = WIO_wifiManager.getPW(i);

    if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
      strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
    else
      strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);

    if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
      strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
    else
      strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);  
  }
}

void setup()
{
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStarting WIOT_ConfigOnStartup on " + String(BOARD_NAME));
  Serial.println(WIO_TERMINAL_WIFIMANAGER_VERSION);

  digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

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

  // If no stored WiFi data, load from saved EEPROM 
  if ( (Router_SSID == "") || (Router_Pass == "") )
  {
    // Load stored data and verify CheckSum
    if (loadConfigData())
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
      {
        // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
        if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
        {
          WIO_LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
          wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);

          Router_SSID = String(WM_config.WiFi_Creds[i].wifi_ssid);
          Router_Pass = String(WM_config.WiFi_Creds[i].wifi_pw);
        }
      }
    }
  }
  
  //Remove this line if you do not want to see WiFi password printed
  WIO_LOGERROR3("Stored: SSID =", Router_SSID, ", Pass =", Router_Pass);

  //Check if there is stored WiFi router/password credentials.
  //If not found, device will remain in configuration mode until switched off via webserver.
  WIO_LOGERROR("Opening configuration portal.");

  // From v1.1.0, Don't permit NULL password
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    WIO_LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
    wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
    
    WIO_wifiManager.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_SEC); //If no access point name has been previously entered disable timeout.
    WIO_LOGERROR1("Got stored Credentials. Timeout for Config Portal =", CONFIG_PORTAL_TIMEOUT_SEC);
  }
  else
  {
    WIO_LOGERROR("Open Config Portal without Timeout: No stored Credentials.");
    digitalWrite(PIN_LED, LED_ON); // Turn led on as we are in configuration mode.

    initialConfig = true;
  }

  WIO_LOGERROR("Starting configuration portal.");
  digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

  // Starts an access point
  if (!WIO_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
  {
    WIO_LOGERROR("Not connected to WiFi but continuing anyway.");
  }
  else
  {
    WIO_LOGERROR("WiFi connected...yeey :)");
  }

  // Only clear then save data if CP entered and with new valid Credentials
  // No CP => stored getSSID() = ""
  if ( String(WIO_wifiManager.getSSID(0)) != "" && String(WIO_wifiManager.getSSID(1)) != "" )
  {
    // Stored  for later usage, from v1.1.0, but clear first
    memset(&WM_config, 0, sizeof(WM_config));
    
    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      String tempSSID = WIO_wifiManager.getSSID(i);
      String tempPW   = WIO_wifiManager.getPW(i);
  
      if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);
  
      if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);  
  
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        WIO_LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }
  
    saveConfigData();

    initialConfig = true;
  }

  digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.

  startedAt = millis();

  if (!initialConfig)
  {
    // Load stored data, the addAP ready for MultiWiFi reconnection
    loadConfigData();

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        WIO_LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    if ( WiFi.status() != WL_CONNECTED ) 
    {
      WIO_LOGERROR("ConnectMultiWiFi in setup");
     
      connectMultiWiFi();
    }
  }

  Serial.print("After waiting ");
  Serial.print((float) (millis() - startedAt) / 1000L);
  Serial.print(" secs more in setup(), connection result is ");

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("connected. Local IP: ");
    Serial.println(WiFi.localIP());
  }
  else
    Serial.println(WIO_wifiManager.getStatus(WiFi.status()));
}

void loop()
{
  // put your main code here, to run repeatedly
  check_status();
}
