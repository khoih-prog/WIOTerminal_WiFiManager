# WIOTerminal_WiFiManager

[![arduino-library-badge](https://www.ardu-badge.com/badge/WIOTerminal_WiFiManager.svg?)](https://www.ardu-badge.com/WIOTerminal_WiFiManager)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/WIOTerminal_WiFiManager.svg)](https://github.com/khoih-prog/WIOTerminal_WiFiManager/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/WIOTerminal_WiFiManager/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/WIOTerminal_WiFiManager.svg)](http://github.com/khoih-prog/WIOTerminal_WiFiManager/issues)

---
---

### Releases v1.2.0

1. Initial Coding for WIO Terminal. Sync with [ESP_WiFiManager v1.2.0](https://github.com/khoih-prog/ESP_WiFiManager)


---
---

This library is based on, modified, bug-fixed and improved from:

1. [`Tzapu WiFiManager`](https://github.com/tzapu/WiFiManager)
2. [`Ken Taylor WiFiManager`](https://github.com/kentaylor/WiFiManager)
3. [`Khoi Hoang's ESP_WiFiManager`](https://github.com/khoih-prog/ESP_WiFiManager)

to provide support to `SeeedStudio SAMD51 WIO-Terminal`.

This is a WiFi Connection manager with fallback web ConfigPortal.
It's using a web ConfigPortal, served from the `WIO-Terminal`, and operating as an access point.

---
---

## Prerequisites

 1. [`Arduino IDE 1.8.13+` for Arduino](https://www.arduino.cc/en/Main/Software)
 2. [`Seeeduino SAMD core 1.8.1+`](https://www.seeedstudio.com/) for SAMD51 Wio Terminal
 3. [`Seeed_Arduino_rpcWiFi library v1.0.0+`](https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi) for WIO-Terminal or boards using **Realtek RTL8720DN WiFi**. To be used with [`Seeed_Arduino_rpcUnified library v2.0.0+`](https://github.com/Seeed-Studio/Seeed_Arduino_rpcUnified).
 4. [`FlashStorage_SAMD library v1.0.0+`](https://github.com/khoih-prog/FlashStorage_SAMD) for SAMD.
 5. [`DoubleResetDetector_Generic v1.0.2+`](https://github.com/khoih-prog/DoubleResetDetector_Generic) if using DRD feature. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/DoubleResetDetector_Generic.svg?)](https://www.ardu-badge.com/DoubleResetDetector_Generic).

---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for `WIOTerminal_WiFiManager`, then select / install the latest version. You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/WIOTerminal_WiFiManager.svg?)](https://www.ardu-badge.com/WIOTerminal_WiFiManager) for more detailed instructions.

### Manual Install

1. Navigate to [WIOTerminal_WiFiManager](https://github.com/khoih-prog/WIOTerminal_WiFiManager) page.
2. Download the latest release `WIOTerminal_WiFiManager-master.zip`.
3. Extract the zip file to `WIOTerminal_WiFiManager-master` directory 
4. Copy the whole `WIOTerminal_WiFiManager-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**WIOTerminal_WiFiManager** library](https://platformio.org/lib/show/xxxxx/WIOTerminal_WiFiManager) by using [**Library Manager**](https://platformio.org/lib/show/xxxxx/WIOTerminal_WiFiManager/installation). Search for [**WIOTerminal_WiFiManager**](https://platformio.org/lib/show/xxxxx/WIOTerminal_WiFiManager) in [Platform.io **Author's Libraries**](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---

### Packages' Patches

 1. ***To be able to automatically detect and display BOARD_NAME on Seeeduino SAMD (XIAO M0, Wio Terminal, etc) boards***, you have to copy the file [Seeeduino SAMD platform.txt](Packages_Patches/Seeeduino/hardware/samd/1.8.1) into Adafruit samd directory (~/.arduino15/packages/Seeeduino/hardware/samd/1.8.1). 

Supposing the Seeeduino SAMD core version is 1.8.1. This file must be copied into the directory:

- `~/.arduino15/packages/Seeeduino/hardware/samd/1.8.1/platform.txt`

Whenever a new version is installed, remember to copy this file into the new version directory. For example, new version is x.yy.zz
This file must be copied into the directory:

- `~/.arduino15/packages/Seeeduino/hardware/samd/x.yy.zz/platform.txt`

---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using xyz-Impl.h instead of standard xyz.cpp, possibly creates certain `Multiple Definitions` Linker error in certain use cases. Although it's simple to just modify several lines of code, either in the library or in the application, the library is adding 2 more source directories

1. **scr_h** for new h-only files
2. **src_cpp** for standard h/cpp files

besides the standard **src** directory.

To use the old standard cpp way, locate this library' directory, then just 

1. **Delete the all the files in src directory.**
2. **Copy all the files in src_cpp directory into src.**
3. Close then reopen the application code in Arduino IDE, etc. to recompile from scratch.

To re-use the new h-only way, just 

1. **Delete the all the files in src directory.**
2. **Copy the files in src_h directory into src.**
3. Close then reopen the application code in Arduino IDE, etc. to recompile from scratch.

---
---

## How It Works

- The [WIOT_ConfigOnSwitch](examples/WIOT_ConfigOnSwitch) example shows how it works and should be used as the basis for a sketch that uses this library.
- The concept of ConfigOnSwitch is that a new `WIO-Terminal` will start a WiFi ConfigPortal when powered up and save the configuration data in non volatile memory. Thereafter, the ConfigPortal will only be started again if a button is pushed on the `WIO-Terminal` module.
- Using any WiFi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point (AP) using configurable SSID and Password (specified in sketch)

```cpp
// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";
``` 
then connect WebBrowser to configurable ConfigPortal IP address, default is 192.168.4.1

- Choose one of the access points scanned, enter password, click ***Save***.
- ESP will restart, then try to connect to the WiFi netwotk using STA-only mode, ***without running the ConfigPortal WebServer and WiFi AP***. See [Accessing manager after connection](https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/15).

---
---

### HOWTO Basic configurations

#### 1. Using default for every configurable parameter

- Include in your sketch

```cpp
#ifdef ESP32
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>

  // From v1.1.0
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;

  #define USE_SPIFFS      true

  #if USE_SPIFFS
    #include <SPIFFS.h>
    FS* filesystem =      &SPIFFS;
    #define FileFS        SPIFFS
    #define FS_Name       "SPIFFS"
  #else
    // Use FFat
    #include <FFat.h>
    FS* filesystem =      &FFat;
    #define FileFS        FFat
    #define FS_Name       "FFat"
  #endif
  //////

  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

  #define LED_BUILTIN       2
  #define LED_ON            HIGH
  #define LED_OFF           LOW

#else
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>

  // From v1.1.0
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;

  #define USE_LITTLEFS      true
  
  #if USE_LITTLEFS
    #include <LittleFS.h>
    FS* filesystem =      &LittleFS;
    #define FileFS        LittleFS
    #define FS_Name       "LittleFS"
  #else
    FS* filesystem =      &SPIFFS;
    #define FileFS        SPIFFS
    #define FS_Name       "SPIFFS"
  #endif
  //////
  
  #define ESP_getChipId()   (ESP.getChipId())
  
  #define LED_ON      LOW
  #define LED_OFF     HIGH
#endif

// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";

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

#define NUM_WIFI_CREDENTIALS      2

typedef struct
{
  WiFi_Credentials  WiFi_Creds [NUM_WIFI_CREDENTIALS];
} WM_Config;

WM_Config         WM_config;

#define  CONFIG_FILENAME              F("/wifi_cred.dat")
//////

#include <WIOTerminal_WiFiManager.h>              //https://github.com/khoih-prog/WIOTerminal_WiFiManager
```
---

#### 2. Using many configurable parameters

- Include in your sketch

```cpp
#ifdef ESP32
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>

  // From v1.1.0
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;

  #define USE_SPIFFS      true

  #if USE_SPIFFS
    #include <SPIFFS.h>
    FS* filesystem =      &SPIFFS;
    #define FileFS        SPIFFS
    #define FS_Name       "SPIFFS"
  #else
    // Use FFat
    #include <FFat.h>
    FS* filesystem =      &FFat;
    #define FileFS        FFat
    #define FS_Name       "FFat"
  #endif
  //////

  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

  #define LED_BUILTIN       2
  #define LED_ON            HIGH
  #define LED_OFF           LOW

#else
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>

  // From v1.1.0
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;

  #define USE_LITTLEFS      true
  
  #if USE_LITTLEFS
    #include <LittleFS.h>
    FS* filesystem = &LittleFS;
    #define FileFS    LittleFS
    #define FS_Name       "LittleFS"
  #else
    FS* filesystem = &SPIFFS;
    #define FileFS    SPIFFS
    #define FS_Name       "SPIFFS"
  #endif
  //////
  
  #define ESP_getChipId()   (ESP.getChipId())
  
  #define LED_ON      LOW
  #define LED_OFF     HIGH
#endif

// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";

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

#define NUM_WIFI_CREDENTIALS      2

typedef struct
{
  WiFi_Credentials  WiFi_Creds [NUM_WIFI_CREDENTIALS];
} WM_Config;

WM_Config         WM_config;

#define  CONFIG_FILENAME              F("/wifi_cred.dat")
//////

// Indicates whether ESP has WiFi credentials saved from previous session
bool initialConfig = false;

// Use false if you don't like to display Available Pages in Information Page of Config Portal
// Comment out or use true to display Available Pages in Information Page of Config Portal
// Must be placed before #include <WIOTerminal_WiFiManager.h>
#define USE_AVAILABLE_PAGES     false

// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/23)
#define USE_WIOTerminal_WiFiManager_NTP     false

// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP          false

// New in v1.0.11
#define USING_CORS_FEATURE          true
//////

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
  // Force DHCP to be true
  #if defined(USE_DHCP_IP)
    #undef USE_DHCP_IP
  #endif
  #define USE_DHCP_IP     true
#else
  // You can select DHCP or Static IP here
  //#define USE_DHCP_IP     true
  #define USE_DHCP_IP     false
#endif

#if ( USE_DHCP_IP || ( defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP ) )
  // Use DHCP
  #warning Using DHCP IP
  IPAddress stationIP   = IPAddress(0, 0, 0, 0);
  IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
  IPAddress netMask     = IPAddress(255, 255, 255, 0);
#else
  // Use static IP
  #warning Using static IP
  #ifdef ESP32
    IPAddress stationIP   = IPAddress(192, 168, 2, 232);
  #else
    IPAddress stationIP   = IPAddress(192, 168, 2, 186);
  #endif
  
  IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
  IPAddress netMask     = IPAddress(255, 255, 255, 0);
#endif

#define USE_CONFIGURABLE_DNS      true

IPAddress dns1IP      = gatewayIP;
IPAddress dns2IP      = IPAddress(8, 8, 8, 8);

#include <WIOTerminal_WiFiManager.h>              //https://github.com/khoih-prog/WIOTerminal_WiFiManager
```

---

#### 3. Using STA-mode DHCP, but don't like to change to static IP or display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
#define USE_STATIC_IP_CONFIG_IN_CP          false

```

---

#### 4. Using STA-mode DHCP, but permit to change to static IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
  // Force DHCP to be true
  #if defined(USE_DHCP_IP)
    #undef USE_DHCP_IP
  #endif
  #define USE_DHCP_IP     true
#else
  // You can select DHCP or Static IP here
  #define USE_DHCP_IP     true
#endif

```

---

#### 5. Using STA-mode StaticIP, and be able to change to DHCP IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
  // Force DHCP to be true
  #if defined(USE_DHCP_IP)
    #undef USE_DHCP_IP
  #endif
  #define USE_DHCP_IP     true
#else
  // You can select DHCP or Static IP here
  #define USE_DHCP_IP     false
#endif

```

---

#### 6. Using STA-mode StaticIP and configurable DNS, and be able to change to DHCP IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
  // Force DHCP to be true
  #if defined(USE_DHCP_IP)
    #undef USE_DHCP_IP
  #endif
  #define USE_DHCP_IP     true
#else
  // You can select DHCP or Static IP here
  #define USE_DHCP_IP     false
#endif

#define USE_CONFIGURABLE_DNS      true

IPAddress dns1IP      = gatewayIP;
IPAddress dns2IP      = IPAddress(8, 8, 8, 8);

```

---

#### 7. Using STA-mode StaticIP and auto DNS, and be able to change to DHCP IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
  // Force DHCP to be true
  #if defined(USE_DHCP_IP)
    #undef USE_DHCP_IP
  #endif
  #define USE_DHCP_IP     true
#else
  // You can select DHCP or Static IP here
  #define USE_DHCP_IP     false
#endif

#define USE_CONFIGURABLE_DNS      false

```

---

#### 8. Not using NTP to avoid issue with some WebBrowsers, especially in CellPhone or Tablets.


```cpp
// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/23)
#define USE_WIOTerminal_WiFiManager_NTP     false

```

---

#### 9. Using NTP feature with CloudFlare. System can hang until you have Internet access for CloudFlare.


```cpp
// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/23)
#define USE_WIOTerminal_WiFiManager_NTP     true

// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP          true

```

---

#### 10. Using NTP feature without CloudFlare to avoid system hang if no Internet access for CloudFlare.


```cpp
// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/23)
#define USE_WIOTerminal_WiFiManager_NTP     true

// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP          false

```

---

#### 11. Using random AP-mode channel to avoid conflict


```cpp
// From v1.0.10 only
// Set config portal channel, default = 1. Use 0 => random channel from 1-13
WIOTerminal_WiFiManager.setConfigPortalChannel(0);
//////
```

---

#### 12. Using fixed AP-mode channel, for example channel 3


```cpp
// From v1.0.10 only
// Set config portal channel, default = 1. Use 0 => random channel from 1-13
WIOTerminal_WiFiManager.setConfigPortalChannel(3);
//////
```
---

#### 13. Setting STA-mode static IP


```cpp
// Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
WIOTerminal_WiFiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
```
---

#### 14. Using AUTOCONNECT_NO_INVALIDATE feature

1. Don't invalidate WiFi SSID/PW when calling autoConnect()  (default)

```cpp
#define AUTOCONNECT_NO_INVALIDATE     true
```

2. To invalidate WiFi SSID/PW when calling autoConnect()

```cpp
#define AUTOCONNECT_NO_INVALIDATE     false
```
---

#### 15. Using CORS (Cross-Origin Resource Sharing) feature

1. To use CORS feature with **default** CORS Header "*". Some WebBrowsers won't accept this allowing-all "*" CORS Header.

```cpp
// Default false for using only whenever necessary to avoid security issue
#define USING_CORS_FEATURE     true
```

2. To use CORS feature with specific CORS Header "Your Access-Control-Allow-Origin". **To be modified** according to your specific Allowed-Origin.

```cpp
// Default false for using only whenever necessary to avoid security issue
#define USING_CORS_FEATURE     true

...

  // New from v1.1.1
#if USING_CORS_FEATURE
  WIOTerminal_WiFiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif
```

3. Not use CORS feature (default)

```cpp
// Default false for using only whenever necessary to avoid security issue
#define USING_CORS_FEATURE     false
```

---

#### 16. Using MultiWiFi auto(Re)connect feature

1. In loop()

```cpp
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
  static ulong checkwifi_timeout    = 0;

  static ulong current_millis;

#define WIFICHECK_INTERVAL    1000L

  current_millis = millis();
  
  // Check WiFi every WIFICHECK_INTERVAL (1) seconds.
  if ((current_millis > checkwifi_timeout) || (checkwifi_timeout == 0))
  {
    check_WiFi();
    checkwifi_timeout = current_millis + WIFICHECK_INTERVAL;
  }
}

void loop()
{
  // put your main code here, to run repeatedly
  check_status();
}
```

---
---

### HOWTO Open Config Portal

- When you want to open a config portal, with default DHCP hostname `ESP8266-XXXXXX` or `ESP32-XXXXXX`, just add

```cpp
WIOTerminal_WiFiManager WIOTerminal_WiFiManager;
```
If you'd like to have a personalized hostname 
`(RFC952-conformed,- 24 chars max,- only a..z A..Z 0..9 '-' and no '-' as last char)`

add

```cpp
WIOTerminal_WiFiManager WIOTerminal_WiFiManager("Personalized-HostName");
```

then later call

```cpp
WIOTerminal_WiFiManager.startConfigPortal()
```

While in AP mode, connect to it using its `SSID` (ESP_XXXXXX) / `Password` ("your_password"), then open a browser to the AP IP, default `192.168.4.1`, configure wifi then save. The WiFi connection information will be saved in non volatile memory. It will then reboot and autoconnect.

You can also change the AP IP by:

```cpp
//set custom ip for portal
WIOTerminal_WiFiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
```

and use fixed / dynamic / random AP channel by:

```cpp
// From v1.0.10 only
// Set config portal channel, default = 1. Use 0 => random channel from 1-13
WIOTerminal_WiFiManager.setConfigPortalChannel(0);
//////
```

Once WiFi network information is saved in the `WIO-Terminal`, it will try to autoconnect to WiFi every time it is started, without requiring any function calls in the sketch.

---
---

### Examples

 1. [WIOT_ConfigOnDoubleReset](examples/WIOT_ConfigOnDoubleReset)
 2. [WIOT_ConfigOnStartup](examples/WIOT_ConfigOnStartup)
 3. [WIOT_ConfigOnSwitch](examples/WIOT_ConfigOnSwitch) 
 4. [WIOT_ConfigOnSwitch_1WiFi](examples/WIOT_ConfigOnSwitch_1WiFi)

---
---

## So, how it works?

In `ConfigPortal Mode`, it starts an access point called `WIOTerminal`. Connect to it using the `configurable password` you can define in the code. For example, `WIOTerminal_Pass` (see examples):

```cpp
// SSID and PW for Config Portal
String ssid = "WIOTerminal";
const char* password = "WIOTerminal_Pass";
```
After you connected, please, go to http://192.168.1.1, you'll see this `Main` page:

<p align="center">
    <img src="https://github.com/khoih-prog/WIOTerminal_WiFiManager/blob/master/Images/Main.png">
</p>

Select `Information` to enter the Info page where the board info will be shown (short page)

<p align="center">
    <img src="https://github.com/khoih-prog/WIOTerminal_WiFiManager/blob/master/Images/Info_Short.png">
</p>

Select `Configuration` to enter this page where you can select an AP and specify its WiFi Credentials

<p align="center">
    <img src="https://github.com/khoih-prog/WIOTerminal_WiFiManager/blob/master/Images/Configuration.png">
</p>

Enter your credentials, then click ***Save***. The WiFi Credentials will be saved and the board reboots to connect to the selected WiFi AP.

<p align="center">
    <img src="https://github.com/khoih-prog/WIOTerminal_WiFiManager/blob/master/Images/Saved.png">
</p>

If you're already connected to a listed WiFi AP and don't want to change anything, just select ***Exit Portal*** from the `Main` page to reboot the board and connect to the previously-stored AP. The WiFi Credentials are still intact.

---
---

## Documentation

#### Password protect the configuration Access Point

You can password protect the ConfigPortal AP.  Simply add an SSID as the first parameter and the password as a second parameter to `startConfigPortal`. See the above examples.
A short password seems to have unpredictable results so use one that's around 8 characters or more in length.
The guidelines are that a wifi password must consist of 8 to 63 ASCII-encoded characters in the range of 32 to 126 (decimal)

```cpp
WIOTerminal_WiFiManager.startConfigPortal( SSID , password )
```

#### Callbacks

##### Save settings

This gets called when custom parameters have been set **AND** a connection has been established. Use it to set a flag, so when all the configuration finishes, you can save the extra parameters somewhere.

See [ConfigOnSwitchFS Example](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnSwitchFS).

```cpp
WIOTerminal_WiFiManager.setSaveConfigCallback(saveConfigCallback);
```
saveConfigCallback declaration and example

```cpp
//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () 
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
```

#### ConfigPortal Timeout

If you need to set a timeout so the `WIO-Terminal` doesn't hang waiting to be configured for ever. 

```cpp
#define CONFIG_PORTAL_TIMEOUT_SEC       120

WIOTerminal_WiFiManager.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_SEC);
```

which will wait 2 minutes (120 seconds). When the time passes, the startConfigPortal function will return and continue the sketch, unless you're accessing the Config Portal. 

In this case, the `startConfigPortal` function will stay until you save config data or exit the Config Portal.


#### On Demand ConfigPortal

Example usage

```cpp
void loop()
{
  // is configuration portal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
  {
    Serial.println("\nConfiguration portal requested.");
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    //Local intialization. Once its business is done, there is no need to keep it around
    WIO_WiFiManager WIO_wifiManager;

    WIO_wifiManager.setMinimumSignalQuality(-1);

    // From v1.0.10 only
    // Set config portal channel, default = 1. Use 0 => random channel from 1-13
    //WIO_wifiManager.setConfigPortalChannel(0);
    //////

    //set custom ip for portal
    //WIO_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

#if !USE_DHCP_IP
#if USE_CONFIGURABLE_DNS
    // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
    //WIO_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
#else
    // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
    //WIO_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask);
#endif
#endif

    // New from v1.1.1
#if USING_CORS_FEATURE
    //WIO_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

    //Check if there is stored WiFi router/password credentials.
    //If not found, device will remain in configuration mode until switched off via webserver.
    Serial.print("Opening configuration portal. ");
    Router_SSID = WIO_wifiManager.getSSID();
    //Router_Pass = WIO_wifiManager.getPassword();

    // From v1.1.0, Don't permit NULL password
    if ( (Router_SSID != "") && (Router_Pass != "") )
    {
      WIO_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
      Serial.println("Got stored Credentials. Timeout 120s");
    }
    else
      Serial.println("No stored Credentials. No timeout");

    //Starts an access point
    //and goes into a blocking loop awaiting configuration
    if (!WIO_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
    {
      Serial.println("Not connected to WiFi but continuing anyway.");
    }
    else
    {
      //if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
    }

    digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.
  }

  // put your main code here, to run repeatedly
  check_status();
}
```

See  [ConfigOnSwitch](examples/WIOT_ConfigOnSwitch) example for a more complex version.

---
---

#### Custom Parameters

Many applications need configuration parameters like `MQTT host and port`, [Blynk](http://www.blynk.cc) or [emoncms](http://emoncms.org) tokens, etc. While it is possible to use `WIOTerminal_WiFiManager` to collect additional parameters it is better to read these parameters from a web service once `WIOTerminal_WiFiManager` has been used to connect to the internet.

To capture other parameters with `WIOTerminal_WiFiManager` is a lot more involved than all the other features and requires adding custom HTML to your form. If you want to do it with `WIOTerminal_WiFiManager` see the example [ConfigOnSwitchFS](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ConfigOnSwitchFS)

#### Custom IP Configuration

You can set a custom IP for both AP (access point, config mode) and STA (station mode, client mode, normal project state)

##### Custom Access Point IP Configuration (currently not working)

This will set your captive portal to a specific IP should you need/want such a feature. Add the following snippet before `startConfigPortal()`

```cpp
//set custom ip for portal
WIOTerminal_WiFiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
```

##### Custom Station (client) Static IP Configuration (currently not working)

This will use the specified IP configuration instead of using DHCP in station mode.
```cpp
WIOTerminal_WiFiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
```

#### Custom HTML, CSS, Javascript

There are various ways in which you can inject custom HTML, CSS or Javascript into the ConfigPortal.

The options are:
- inject custom head element
You can use this to any html bit to the head of the ConfigPortal. If you add a `<style>` element, bare in mind it overwrites the included css, not replaces.

```cpp
WIOTerminal_WiFiManager.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");
```

- inject a custom bit of html in the configuration form

```cpp
WIO_WMParameter custom_text("<p>This is just a text paragraph</p>");
WIOTerminal_WiFiManager.addParameter(&custom_text);
```

- inject a custom bit of html in a configuration form element
Just add the bit you want added as the last parameter to the custom parameter constructor.

```cpp
WIO_WMParameter custom_mqtt_server("server", "mqtt server", "iot.eclipse", 40, " readonly");
```

#### Filter Networks

You can filter networks based on signal quality and show/hide duplicate networks.

- If you would like to filter low signal quality networks you can tell WiFiManager to not show networks below an arbitrary quality %;

```cpp
WIOTerminal_WiFiManager.setMinimumSignalQuality(10);
```
will not show networks under 10% signal quality. If you omit the parameter it defaults to 8%;

- You can also remove or show duplicate networks (default is remove).
Use this function to show (or hide) all networks.

```cpp
WIOTerminal_WiFiManager.setRemoveDuplicateAPs(false);
```
---
---

### Example [WIOT_ConfigOnDoubleReset](examples/WIOT_ConfigOnDoubleReset)

```cpp
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

#define DRD_FLAG_DATA_SIZE      4

#define DRD_GENERIC_DEBUG       true  //false

#include <DoubleResetDetector_Generic.h>      //https://github.com/khoih-prog/DoubleResetDetector_Generic

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT             10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS             0

//DoubleResetDetector_Generic drd(DRD_TIMEOUT, DRD_ADDRESS);
DoubleResetDetector_Generic* drd;//////

#ifndef EEPROM_START
  #define EEPROM_START     DRD_FLAG_DATA_SIZE
#endif

// SSID and PW for Config Portal
String ssid = "WIOTerminal";
const char* password = "WIOTerminal_Pass";

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
#define USING_CORS_FEATURE          true
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

  Serial.println("\nStarting WIOT_ConfigOnDoubleReset with DoubleResetDetect on " + String(BOARD_NAME));
  Serial.println(WIO_TERMINAL_WIFIMANAGER_VERSION);

  drd = new DoubleResetDetector_Generic(DRD_TIMEOUT, DRD_ADDRESS);

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

  if (drd->detectDoubleReset())
  {
    // DRD, disable timeout.
    WIO_wifiManager.setConfigPortalTimeout(0);

    WIO_LOGERROR("Open Config Portal without Timeout: Double Reset Detected");
    initialConfig = true;
  }
  else
  {
    WIO_LOGERROR("No Double Reset Detected");
  }

  // We can't use WiFi.SSID() in ESP32as it's only valid after connected.
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS for this purpose
  Router_SSID = WIO_wifiManager.WiFi_SSID();
  Router_Pass = WIO_wifiManager.WiFi_Pass();

  //Remove this line if you do not want to see WiFi password printed
  Serial.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

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
        }
      }
  
      if ( ( WiFi.status() != WL_CONNECTED ) && !initialConfig )
      {
        WIO_LOGERROR(F("ConnectWiFi in setup"));
  
        if ( connectMultiWiFi() == WL_CONNECTED )
        {
          Serial.println("Connected to " + Router_SSID);
          return;
        }
      }
    }
  }
  
  // From v1.1.0, Don't permit NULL password
  if ( (Router_SSID != "") && (Router_Pass != "")  && !initialConfig )
  {
    WIO_LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
    wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
    
    WIO_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
    WIO_LOGERROR("Got stored Credentials. Timeout 120s for Config Portal");
  }
  else
  {
    WIO_LOGERROR("Open Config Portal without Timeout: No stored Credentials.");
    digitalWrite(PIN_LED, LED_ON); // Turn led on as we are in configuration mode.

    initialConfig = true;
  }

  if (initialConfig)
  {
    WIO_LOGERROR("Starting configuration portal.");
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    //sets timeout in seconds until configuration portal gets turned off.
    //If not specified device will remain in configuration mode until
    //switched off via webserver or device is restarted.
    //WIO_wifiManager.setConfigPortalTimeout(600);

    // Starts an access point
    if (!WIO_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
    {
      WIO_LOGERROR("Not connected to WiFi but continuing anyway.");
    }
    else
    {
      WIO_LOGERROR("WiFi connected...yeey :)");
    }

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
  // Call the double reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call drd.stop() when you wish to no longer
  // consider the next reset as a double reset.
  drd->loop();

  // put your main code here, to run repeatedly
  check_status();

}
```

---
---

### Debug Termimal Output Samples

1. This is terminal debug output when running [WIOT_ConfigOnDoubleReset](examples/WIOT_ConfigOnDoubleReset) on  ***SeeedStudio SAMD51 WIO-Terminal***. 

Config Portal (CP) was requested to input and save WiFi Credentials. The boards then connected to WiFi successfully. Then DRD was detected, CP was again requested to update WiFi Credentials.

#### First start without WiFi Credentials => enter Config Portal (CP)

```
Starting WIOT_ConfigOnDoubleReset with DoubleResetDetect on WIO_TERMINAL
WIOTerminal_WiFiManager v1.2.0
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
Flag read = 0xffffffff
No doubleResetDetected
SetFlag write = 0xd0d01234
[WM] No Double Reset Detected
Stored: SSID = , Pass = 
Calc. CheckSum = 0xbf40, Read CheckSum = 0xffffffff
[WM] Open Config Portal without Timeout: No stored Credentials.
[WM] Starting configuration portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP STA
[WM] new DNSServer
[WM] new WebServer
[WM] 
Configuring AP SSID = WIOTerminal
[WM] AP PWD = WIOTerminal_Pass
[WM] AP Channel = 1
[WM] AP IP address = 192.168.1.1
[WM] HTTP server started
[WM] WIO_WiFiManager::startConfigPortal : Enter loop
[WM] WiFi save
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Removing Wi-Fi Settings
[WM] Connect to new WiFi using new IP parameters
[WM] Connection result:  WL_CONNECTED
[WM] WiFi connected...yeey :)
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
CheckSum = 0xbc7
After waiting 0.00 secs more in setup(), connection result is connected. Local IP: 192.168.2.150
Stop doubleResetDetecting
ClearFlag write = 0xd0d04321
H
```

#### DRD detected, Config Portal will open

```
Starting WIOT_ConfigOnDoubleReset with DoubleResetDetect on WIO_TERMINAL
WIOTerminal_WiFiManager v1.2.0
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
Flag read = 0xd0d01234
doubleResetDetected
ClearFlag write = 0xd0d04321
[WM] Open Config Portal without Timeout: Double Reset Detected
Stored: SSID = , Pass = 
Calc. CheckSum = 0xbc7, Read CheckSum = 0xbc7
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
[WM] Open Config Portal without Timeout: No stored Credentials.
[WM] Starting configuration portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP STA
[WM] new DNSServer
[WM] new WebServer
[WM] 
Configuring AP SSID = WIOTerminal
[WM] AP PWD = WIOTerminal_Pass
[WM] AP Channel = 1
[WM] AP IP address = 192.168.1.1
[WM] HTTP server started
[WM] WIO_WiFiManager::startConfigPortal : Enter loop
[WM] WiFi save
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Removing Wi-Fi Settings
[WM] Connect to new WiFi using new IP parameters
[WM] Connection result:  WL_CONNECTED
[WM] WiFi connected...yeey :)
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
CheckSum = 0xbc7
After waiting 0.00 secs more in setup(), connection result is connected. Local IP: 192.168.2.150
H
```
---

2. This is terminal debug output when running [WIOT_ConfigOnSwitch](examples/WIOT_ConfigOnSwitch) on  ***SeeedStudio SAMD51 WIO-Terminal***.

By pressing a Switch (WIO_KEY_C or WIO_KEY_B), Config Portal (CP) was requested in loop() to update and save WiFi Credentials.

The boards then connected to WiFi successfully. 

#### First start without WiFi Credentials => enter Config Portal (CP)

```cpp
Starting WIOT_ConfigOnSwitch on WIO_TERMINAL
WIOTerminal_WiFiManager v1.2.0
[WM] Calc. CheckSum = 0xbf40 , Read CheckSum = 0xffffffff  **<======== Restart and No stored data in Flash**
Stored: SSID = , Pass = 
Open Config Portal without Timeout: No stored Credentials.
Starting Config. Portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP STA
[WM] 
Configuring AP SSID = WIOTerminal
[WM] AP PWD = WIOTerminal_Pass
[WM] AP Channel = 1
[WM] AP IP address = 192.168.1.1
[WM] HTTP server started
[WM] WIO_WiFiManager::startConfigPortal : Enter loop
[WM] WiFi save
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Removing Wi-Fi Settings
[WM] Connect to new WiFi using new IP parameters
[WM] Connection result:  WL_CONNECTED
WiFi connected...yeey :)
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
[WM] CheckSum=0xbc7
After 37.85 secs waiting in setup(), result is connected. Local IP: 192.168.2.150
HHHHHHH
```

#### WiFi Lost and reconnect to secondary AP

```
WiFi lost. Call connectMultiWiFi in loop
[WM] ConnectMultiWiFi with :
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID:  ,RSSI= -56
[WM] IP address: 192.168.2.150
HH
```

#### Restart and Read WiFi Credentials from Flash
           
```
Starting ConfigOnSwitch on WIO_TERMINAL
[WM] Calc. CheckSum = 0xbc7 , Read CheckSum = 0xbc7
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID:  ,RSSI= -43
[WM] IP address: 192.168.2.150
HHHHHHHHHH 
```

---

3. This is terminal debug output when running [WIOT_ConfigOnSwitch_1WiFi](examples/WIOT_ConfigOnSwitch_1WiFi) on  ***SeeedStudio SAMD51 WIO-Terminal*** to demonstrate how to configure the library to work with only single WiFi.

By pressing a Switch (WIO_KEY_C or WIO_KEY_B), Config Portal (CP) was requested in loop() to update and save WiFi Credentials.

The boards then connected to WiFi successfully. 

#### First start without WiFi Credentials => enter Config Portal (CP)

```cpp
Starting WIOT_ConfigOnSwitch_1WiFi on WIO_TERMINAL
WIOTerminal_WiFiManager v1.2.0
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
Calc. CheckSum = 0x5fa0, Read CheckSum = 0xffffffff
Stored: SSID = , Pass = 
Open Config Portal without Timeout: No stored Credentials.
Starting Config. Portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP STA
[WM] new DNSServer
[WM] new WebServer
[WM] 
Configuring AP SSID = WIOTerminal
[WM] AP PWD = WIOTerminal_Pass
[WM] AP Channel = 1
[WM] AP IP address = 192.168.1.1
[WM] HTTP server started
[WM] WIO_WiFiManager::startConfigPortal : Enter loop
[WM] Handle root
[WM] Handle Wifi
[WM] Scanning Network
[WM] scanWifiNetworks: Done, Scanned Networks n = 19
[WM] Sorting
[WM] Removing Dup
[WM] DUP AP: 
[WM] DUP AP: 
[WM] DUP AP: 
[WM] Index = 0
[WM] SSID = HueNet_5G
[WM] RSSI = -29
[WM] Index = 1
[WM] SSID = HueNetTek_5G
[WM] RSSI = -29
[WM] Index = 2
[WM] SSID = HueNet
[WM] RSSI = -31
[WM] Index = 3
[WM] SSID = HueNet1
[WM] RSSI = -32
[WM] Index = 4
[WM] SSID = HueNetTek
[WM] RSSI = -32
[WM] Index = 5
[WM] SSID = HueNet2_5G
[WM] RSSI = -48
[WM] Index = 6
[WM] SSID = HueNet2
[WM] RSSI = -55
[WM] Index = 7
[WM] SSID = guest_5
[WM] RSSI = -71
[WM] Index = 8
[WM] SSID = pitesti
[WM] RSSI = -72
[WM] Index = 9
[WM] SSID = 
[WM] RSSI = -77
[WM] Index = 10
[WM] SSID = guest_24
[WM] RSSI = -81
[WM] Index = 12
[WM] SSID = Waterhome
[WM] RSSI = -88
[WM] Index = 15
[WM] SSID = Oceanlife
[WM] RSSI = -89
[WM] Index = 16
[WM] SSID = Access
[WM] RSSI = -94
[WM] Index = 17
[WM] SSID = dlink-4F96
[WM] RSSI = -95
[WM] Index = 18
[WM] SSID = BELL246
[WM] RSSI = -99
[WM] Sent config page
[WM] WiFi save
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Removing Wi-Fi Settings
[WM] Connect to new WiFi using new IP parameters
[WM] Connection result:  WL_CONNECTED
WiFi connected...yeey :)
CheckSum = 0x5e3
After 68.39 secs waiting in setup(), result is connected. Local IP: 192.168.2.150
H
```

#### Reset => Auto reconnect

```
Starting WIOT_ConfigOnSwitch_1WiFi on WIO_TERMINAL
WIOTerminal_WiFiManager v1.2.0
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
Calc. CheckSum = 0x5e3, Read CheckSum = 0x5e3
[WM] * Add SSID =  HueNet1 , PW =  12345678
ConnectWiFi in setup
Connecting to HueNet1
Connected to HueNet1
H
```

#### Press WIO_KEY_C or WIO_KEY_B to Ener Config Portal

```
Config. portal requested.
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
Opening configuration portal. No stored Credentials. No timeout
[WM] WiFi.waitForConnectResult Done
[WM] SET AP STA
[WM] new DNSServer
[WM] new WebServer
[WM] 
Configuring AP SSID = WIOTerminal
[WM] AP PWD = WIOTerminal_Pass
[WM] AP Channel = 1
[WM] AP IP address = 192.168.1.1
[WM] HTTP server started
[WM] WIO_WiFiManager::startConfigPortal : Enter loop
```

#### Save Credentials fron CP

```
[WM] WiFi save
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Removing Wi-Fi Settings
[WM] Connect to new WiFi using new IP parameters
[WM] Connection result:  WL_CONNECTED
connected...yeey :)
Local IP: 192.168.2.150
CheckSum = 0x5e3
H
```

---

4. This is terminal debug output when running [WIOT_ConfigOnStartup](examples/WIOT_ConfigOnStartup) on  ***SeeedStudio SAMD51 WIO-Terminal***.

This example will open a configuration portal for CONFIG_PORTAL_TIMEOUT_SEC seconds when first powered up if the boards has stored WiFi Credentials.

Otherwise, it'll stay indefinitely in ConfigPortal until getting WiFi Credentials and connecting to WiFi

#### No stored Credentials, Config Portal will open without timeout

```cpp
Starting WIOT_ConfigOnStartup on WIO_TERMINAL
WIOTerminal_WiFiManager v1.2.0
Calc. CheckSum = 0xbf40, Read CheckSum = 0xffffffff
[WM] Stored: SSID =  , Pass = 
[WM] Opening configuration portal.
[WM] Open Config Portal without Timeout: No stored Credentials.
[WM] Starting configuration portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP STA
[WM] new DNSServer
[WM] new WebServer
[WM] 
Configuring AP SSID = WIOTerminal
[WM] AP PWD = WIOTerminal_Pass
[WM] AP Channel = 1
[WM] AP IP address = 192.168.1.1
[WM] HTTP server started
[WM] WIO_WiFiManager::startConfigPortal : Enter loop
[WM] WiFi save
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Removing Wi-Fi Settings
[WM] Connect to new WiFi using new IP parameters
[WM] Connection result:  WL_CONNECTED
[WM] WiFi connected...yeey :)
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
CheckSum = 0xbc7
After waiting 0.00 secs more in setup(), connection result is connected. Local IP: 192.168.2.150
H
```

#### Restarted, Config Portal will open with 120s timeout

```
Starting ConfigOnStartup on WIO_TERMINAL
WIOTerminal_WiFiManager v1.2.0
Calc. CheckSum = 0xbc7, Read CheckSum = 0xbc7
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
[WM] Stored: SSID = HueNet2 , Pass = 12345678
[WM] Opening configuration portal.
[WM] * Add SSID =  HueNet2 , PW =  12345678
[WM] Got stored Credentials. Timeout for Config Portal = 120
[WM] Starting configuration portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP STA
[WM] new DNSServer
[WM] new WebServer
[WM] 
Configuring AP SSID = WIOTerminal
[WM] AP PWD = WIOTerminal_Pass
[WM] AP Channel = 1
[WM] AP IP address = 192.168.1.1
[WM] HTTP server started
[WM] WIO_WiFiManager::startConfigPortal : Enter loop
[WM] Timed out connection result: WL_CONNECTED
[WM] WiFi connected...yeey :)
Calc. CheckSum = 0xbc7, Read CheckSum = 0xbc7
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
After waiting 0.05 secs more in setup(), connection result is connected. Local IP: 192.168.2.150
HH

```

---
---

### Debug

Debug is enabled by default on Serial. To disable, add before `startConfigPortal()`

```cpp
WIOTerminal_WiFiManager.setDebugOutput(false);
```

You can also change the debugging level from 0 to 4

```cpp
// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIO_WIFIMGR_LOGLEVEL_    3
```
---
---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install newer versions of 

1. [`Seeeduino SAMD core 1.8.1+`](https://www.seeedstudio.com/) for SAMD51 Wio Terminal
2. [`Seeed_Arduino_rpcWiFi library v1.0.0+`](https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi) for WIO-Terminal or boards using **Realtek RTL8720DN WiFi**. To be used with [`Seeed_Arduino_rpcUnified library v2.0.0+`](https://github.com/Seeed-Studio/Seeed_Arduino_rpcUnified).the `WIO-Terminal` core for Arduino.

Sometimes, the library will only work if you update the `Seeed_Arduino_rpcUnified` or `Seeed_Arduino_rpcWiFi` library to the latest version because I am using some newly added function.

If you connect to the created configuration Access Point but the ConfigPortal does not show up, just open a browser and type in the IP of the web portal, by default `192.168.1.1`.

---

### Issues ###

Submit issues to: [WIOTerminal_WiFiManager issues](https://github.com/khoih-prog/WIOTerminal_WiFiManager/issues)

---
---

### Releases v1.2.0

1. Initial Coding for WIO Terminal. Sync with [ESP_WiFiManager v1.2.0](https://github.com/khoih-prog/ESP_WiFiManager)

---
---

### CURRENT LIMITATIONS

Due to some problems of still-unmature `Seeed_Arduino_rpcUnified` and `Seeed_Arduino_rpcWiFi` libraries, there are the current limitations:

1. Hostname and StaticIP not working.
2. AP IP Address is only `192.168.1.1`
3. Some new versions of `Seeed_Arduino_rpcUnified` and `Seeed_Arduino_rpcWiFi` will break the WiFi Scanning process and hang there.


### TO DO

1. Bug Searching and Killing
2. Support more types of new boards
3. Support more types of Ethernet modules/shields, such as **LAN8742A, LAN8720, etc.**
4. Support more types of WiFi modules/shields, such as WiFi101, **ESP8266-AT, ESP32-AT**
5. Support **GSM/GPRS** modules/shields.
6. Support ENC28J60 Ethernet modules/shields, using new [EthernetENC](https://github.com/jandrassy/EthernetENC) library.

### DONE

 1. Add support to **Adafruit SAMD21 (Itsy-Bitsy M0, Metro M0, Feather M0 Express, etc.)**.
 2. Add support to **Adafruit SAMD51 (Itsy-Bitsy M4, Metro M4, Grand Central M4, Feather M4 Express, etc.)**.
 3. Add support to **Adafruit nRF52 ( Feather nRF52832, nRF52840 Express, BlueFruit Sense, Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, NINA_B302_ublox, NINA_B112_ublox, etc.)**.
 4. Add support to SAM DUE.
 5. Add support to Teensy.
 6. Add support to all **STM32F/L/H/G/WB/MP1 having 64K+ Flash program memory.**
 7. Add support to **Seeeduino SAMD21/SAMD51 boards (SEEED_WIO_TERMINAL, SEEED_FEMTO_M0, SEEED_XIAO_M0, Wio_Lite_MG126, WIO_GPS_BOARD, SEEEDUINO_ZERO, SEEEDUINO_LORAWAN, SEEED_GROVE_UI_WIRELESS, etc.)**
 8. Add support to **W5x00, ENC28J60** Ethernet modules/shields.
 9. Add support to **Ethernet, EthernetLarge, Ethernet2, Ethernet3, UIPEthernet** libraries.
 
---
---

### Contributions and Thanks

1. Based on and modified from [Tzapu](https://github.com/tzapu/WiFiManager) and [KenTaylor's version]( https://github.com/kentaylor/WiFiManager)


<table>
  <tr>
    <td align="center"><a href="https://github.com/Tzapu"><img src="https://github.com/Tzapu.png" width="100px;" alt="Tzapu"/><br /><sub><b>⭐️ Tzapu</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/kentaylor"><img src="https://github.com/kentaylor.png" width="100px;" alt="kentaylor"/><br /><sub><b>⭐️ Ken Taylor</b></sub></a><br /></td>
  </tr> 
</table>

---

### Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License and credits ###

- The library is licensed under [MIT](https://github.com/khoih-prog/WIOTerminal_WiFiManager/blob/master/LICENSE)

---

## Copyright

Copyright 2020- Khoi Hoang


