/****************************************************************************************************************************
  WIOTerminal_WiFiManager-Impl.h
  For WIO Terminal boards

  WIOTerminal_WiFiManager is a library for the SeeedStudiocWIO Terminal/Arduino platform

  Modified from 
  1) Tzapu        https://github.com/tzapu/WiFiManager
  2) Ken Taylor   https://github.com/kentaylor
  3) Khoi Hoang   https://github.com/khoih-prog/ESP_WiFiManager

  Built by Khoi Hoang https://github.com/khoih-prog/WIOTerminal_WiFiManager
  Licensed under MIT license
  Version: 1.2.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.0   K Hoang      14/12/2020 Initial Coding for WIO Terminal. Sync with ESP_WiFiManager v1.2.0
  1.2.1   K Hoang      13/10/2021 Update `platform.ini` and `library.json`
 *****************************************************************************************************************************/

#pragma once

//////////////////////////////////////////

WIO_WMParameter::WIO_WMParameter(const char *custom)
{
  _id = NULL;
  _placeholder = NULL;
  _length = 0;
  _value = NULL;
  _labelPlacement = WFM_LABEL_BEFORE;

  _customHTML = custom;
}

///////////////////////////////////////////////////

WIO_WMParameter::WIO_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length)
{
  init(id, placeholder, defaultValue, length, "", WFM_LABEL_BEFORE);
}

///////////////////////////////////////////////////

WIO_WMParameter::WIO_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom)
{
  init(id, placeholder, defaultValue, length, custom, WFM_LABEL_BEFORE);
}

///////////////////////////////////////////////////

WIO_WMParameter::WIO_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement)
{
  init(id, placeholder, defaultValue, length, custom, labelPlacement);
}

///////////////////////////////////////////////////

void WIO_WMParameter::init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement)
{
  _id = id;
  _placeholder = placeholder;
  _length = length;
  _labelPlacement = labelPlacement;

  _value = new char[_length + 1];

  if (_value != NULL)
  {
    memset(_value, 0, _length + 1);

    if (defaultValue != NULL)
    {
      strncpy(_value, defaultValue, _length);
    }
  }
  _customHTML = custom;
}

///////////////////////////////////////////////////

WIO_WMParameter::~WIO_WMParameter()
{
  if (_value != NULL)
  {
    delete[] _value;
  }
}

///////////////////////////////////////////////////

const char* WIO_WMParameter::getValue()
{
  return _value;
}

///////////////////////////////////////////////////

const char* WIO_WMParameter::getID()
{
  return _id;
}

///////////////////////////////////////////////////

const char* WIO_WMParameter::getPlaceholder()
{
  return _placeholder;
}

///////////////////////////////////////////////////

int WIO_WMParameter::getValueLength()
{
  return _length;
}

///////////////////////////////////////////////////

int WIO_WMParameter::getLabelPlacement()
{
  return _labelPlacement;
}

///////////////////////////////////////////////////

const char* WIO_WMParameter::getCustomHTML()
{
  return _customHTML;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////

WIO_WiFiManager::WIO_WiFiManager() 
{
}

///////////////////////////////////////////////////

WIO_WiFiManager::~WIO_WiFiManager()
{  
}

///////////////////////////////////////////////////

void WIO_WiFiManager::addParameter(WIO_WMParameter *p) 
{
  if(_paramsCount + 1 > WIFI_MANAGER_MAX_PARAMS)
  {
    //Max parameters exceeded!
	WIO_LOGDEBUG("WIFI_MANAGER_MAX_PARAMS exceeded, increase number (in WIO_WiFiManager.h) before adding more parameters!");
	WIO_LOGDEBUG1("Skipping parameter with ID:", p->getID());
	return;
  }
  
  _params[_paramsCount] = p;
  _paramsCount++;
  WIO_LOGDEBUG1("Adding parameter", p->getID());
}

////////////////////////////////////////////

void WIO_WiFiManager::setupConfigPortal()
{
  stopConfigPortal = false; //Signal not to close config portal

  /*This library assumes autoconnect is set to 1. It usually is
    but just in case check the setting and turn on autoconnect if it is off.
    Some useful discussion at https://github.com/esp8266/Arduino/issues/1615*/
  if (WiFi.getAutoConnect() == 0)
    WiFi.setAutoConnect(1);
 
  if (!dnsServer)
  {
    dnsServer.reset(new DNSServer());
    WIO_LOGWARN(F("new DNSServer"));
  }

  if (!server)
  {
    server.reset(new WebServer(80));
    WIO_LOGWARN(F("new WebServer"));
  }
 
  /* Setup the DNS server redirecting all the domains to the apIP */
  if (dnsServer)
  {
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());
  }

  _configPortalStart = millis();

  WIO_LOGWARN1(F("\nConfiguring AP SSID ="), _apName);

  if (_apPassword != NULL)
  {
    if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63)
    {
      // fail passphrase to short or long!
      WIO_LOGERROR(F("Invalid AccessPoint password. Ignoring"));
      
      _apPassword = NULL;
    }
    
    WIO_LOGWARN1(F("AP PWD ="), _apPassword);
  }
  
  
  // KH, new from v1.0.10 to enable dynamic/random channel
  static int channel;
  // Use random channel if  _WiFiAPChannel == 0
  if (_WiFiAPChannel == 0)
    channel = (_configPortalStart % MAX_WIFI_CHANNEL) + 1;
  else
    channel = _WiFiAPChannel;
  
  if (_apPassword != NULL)
  {
    WIO_LOGWARN1(F("AP Channel ="), channel);
    
    //WiFi.softAP(_apName, _apPassword);//password option
    WiFi.softAP(_apName, _apPassword, channel);
  }
  else
  {
    // Can't use channel here
    WiFi.softAP(_apName);
  }
  //////
  
  // From v1.0.11
  // Contributed by AlesSt (https://github.com/AlesSt) to solve issue softAP with custom IP sometimes not working
  // See https://github.com/khoih-prog/WIO_WiFiManager/issues/26 and https://github.com/espressif/arduino-esp32/issues/985
  // delay 100ms to wait for SYSTEM_EVENT_AP_START
  delay(100);
  //////
  
  //optional soft ip config
  if (_ap_static_ip)
  {
    WIO_LOGWARN(F("Custom AP IP/GW/Subnet = "));
    WIO_LOGWARN2(_ap_static_ip, _ap_static_gw, _ap_static_sn);
    
    WiFi.softAPConfig(_ap_static_ip, _ap_static_gw, _ap_static_sn);
  }

  delay(500); // Without delay I've seen the IP address blank
  
  WIO_LOGWARN1(F("AP IP address ="), WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server->on("/", std::bind(&WIO_WiFiManager::handleRoot, this));
  //server->on("/wifi", std::bind(&WIO_WiFiManager::handleWifi, this));
  server->on("/wifi", std::bind(&WIO_WiFiManager::handleWifi, this, true));
  server->on("/0wifi", std::bind(&WIO_WiFiManager::handleWifi, this, false));
  server->on("/wifisave", std::bind(&WIO_WiFiManager::handleWifiSave, this));
  server->on("/close", std::bind(&WIO_WiFiManager::handleServerClose, this));
  server->on("/i", std::bind(&WIO_WiFiManager::handleInfo, this));
  server->on("/r", std::bind(&WIO_WiFiManager::handleReset, this));
  //server->on("/state", std::bind(&WIO_WiFiManager::handleState, this));
  //server->on("/scan", std::bind(&WIO_WiFiManager::handleScan, this));
  server->onNotFound(std::bind(&WIO_WiFiManager::handleNotFound, this));
  server->begin(); // Web server start
  
  WIO_LOGWARN(F("HTTP server started"));
}

///////////////////////////////////////////////////////////

bool WIO_WiFiManager::autoConnect() 
{
#if defined(WIO_TERMINAL)
  String ssid = "WT-" + WioTerminalID();
#else
  String ssid = "ESP" + String(ESP_getChipId());
#endif
  return autoConnect(ssid.c_str(), NULL);
}

///////////////////////////////////////////////////

bool WIO_WiFiManager::autoConnect(char const *apName, char const *apPassword) 
{
  WIO_LOGDEBUG(F("AutoConnect"));

  // read eeprom for ssid and pass
  //String ssid = getSSID();
  //String pass = getPassword();

  // attempt to connect; should it fail, fall back to AP
  WiFi.mode(WIFI_STA);

  if (connectWifi("", "") == WL_CONNECTED)   
  {
    WIO_LOGDEBUG1(F("IP Address:"), WiFi.localIP());

    //connected
    return true;
  }

  return startConfigPortal(apName, apPassword);
}

///////////////////////////////////////////////////

bool WIO_WiFiManager::configPortalHasTimeout()
{
    if(_configPortalTimeout == 0)
    {  
      // TODO
      _configPortalStart = millis(); // kludge, bump configportal start time to skew timeouts
      return false;
    }
    
    return (millis() > _configPortalStart + _configPortalTimeout);
}

///////////////////////////////////////////////////

bool WIO_WiFiManager::startConfigPortal() 
{
#if defined(WIO_TERMINAL)
  String ssid = "WT-" + WioTerminalID();
#else
  String ssid = "ESP" + String(ESP_getChipId());
#endif
  return startConfigPortal(ssid.c_str(), NULL);
}

////////////////////////////////////////////////////////////

bool  WIO_WiFiManager::startConfigPortal(char const *apName, char const *apPassword)
{
  //setup AP
  int connRes = WiFi.waitForConnectResult();

  WIO_LOGINFO("WiFi.waitForConnectResult Done");

  //setup AP
  // KH, Must have this for WIO Terminal
  wifi_disconnect();
  //////
  
  WiFi.mode(WIFI_AP_STA);
  WIO_LOGDEBUG("SET AP STA");
  
  _apName = apName;
  _apPassword = apPassword;

  //notify we entered AP mode
  if (_apcallback != NULL)
  {
    WIO_LOGINFO("_apcallback");
    
    _apcallback(this);
  }

  connect = false;

  setupConfigPortal();

  bool TimedOut = true;

  WIO_LOGINFO("WIO_WiFiManager::startConfigPortal : Enter loop");

  while (_configPortalTimeout == 0 || millis() < _configPortalStart + _configPortalTimeout)
  {
    //DNS
    dnsServer->processNextRequest();
    //HTTP
    server->handleClient();
    
    //WIO_LOGERROR(F("s"));

    if (connect)
    {
      TimedOut = false;
      //delay(2000);

      WIO_LOGERROR(F("Connecting to new AP"));

      // using user-provided  _ssid, _pass in place of system-stored ssid and pass
      if (connectWifi(_ssid, _pass) != WL_CONNECTED)
      {  
        WIO_LOGERROR(F("Failed to connect"));
    
        //WiFi.mode(WIFI_AP); // Dual mode becomes flaky if not connected to a WiFi network.
      }
      else
      {
        //notify that configuration has changed and any optional parameters should be saved
        if (_savecallback != NULL)
        {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }
        break;
      }

      if (_shouldBreakAfterConfig)
      {
        //flag set to exit after config after trying to connect
        //notify that configuration has changed and any optional parameters should be saved
        if (_savecallback != NULL)
        {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }
        break;
      }
    }

    if (stopConfigPortal)
    {
      WIO_LOGERROR("Stop ConfigPortal");  	//KH
     
      stopConfigPortal = false;
      break;
    }
    
    yield();
  }

  //WiFi.mode(WIFI_STA);
  
  if (TimedOut)
  {
    //setHostname();

    // New v1.0.8 to fix static IP when CP not entered or timed-out
    //setWifiStaticIP();
    
    WiFi.begin();
    int connRes = waitForConnectResult();

    WIO_LOGERROR1("Timed out connection result:", getStatus(connRes));
  }

  server->stop();
  server.reset();
  dnsServer->stop();
  dnsServer.reset();

  return  WiFi.status() == WL_CONNECTED;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setWifiStaticIP()
{ 
#if USE_CONFIGURABLE_DNS
  if (_sta_static_ip)
  {
    WIO_LOGWARN(F("Custom STA IP/GW/Subnet"));
   
    //***** Added section for DNS config option *****
    if (_sta_static_dns1 && _sta_static_dns2) 
    { 
      WIO_LOGWARN(F("DNS1 and DNS2 set"));
 
      WiFi.config(_sta_static_ip, _sta_static_gw, _sta_static_sn, _sta_static_dns1, _sta_static_dns2);
    }
    else if (_sta_static_dns1) 
    {
      WIO_LOGWARN(F("Only DNS1 set"));
     
      WiFi.config(_sta_static_ip, _sta_static_gw, _sta_static_sn, _sta_static_dns1);
    }
    else 
    {
      WIO_LOGWARN(F("No DNS server set"));
  
      WiFi.config(_sta_static_ip, _sta_static_gw, _sta_static_sn);
    }
    //***** End added section for DNS config option *****

    WIO_LOGINFO1(F("setWifiStaticIP IP ="), WiFi.localIP());
  }
  else
  {
    WIO_LOGWARN(F("Can't use Custom STA IP/GW/Subnet"));
  }
#else
  // check if we've got static_ip settings, if we do, use those.
  if (_sta_static_ip)
  {
    WiFi.config(_sta_static_ip, _sta_static_gw, _sta_static_sn);
    
    WIO_LOGWARN1(F("Custom STA IP/GW/Subnet : "), WiFi.localIP());
  }
#endif
}

///////////////////////////////////////////////////


// New from v1.1.0
int WIO_WiFiManager::reconnectWifi(void)
{
  int connectResult;
  
  // using user-provided  _ssid, _pass in place of system-stored ssid and pass
  if ( ( connectResult = connectWifi(_ssid, _pass) ) != WL_CONNECTED)
  {  
    WIO_LOGERROR1(F("Failed to connect to"), _ssid);
    
#if (!defined(USING_ONE_WIFI) || !USING_ONE_WIFI)       
    if ( ( connectResult = connectWifi(_ssid1, _pass1) ) != WL_CONNECTED)
    {  
      WIO_LOGERROR1(F("Failed to connect to"), _ssid1);

    }
    else
      WIO_LOGERROR1(F("Connected to"), _ssid1);
#endif
      
  }
  else
      WIO_LOGERROR1(F("Connected to"), _ssid);
  
  return connectResult;
}

//////////////////////////////////////////

int WIO_WiFiManager::connectWifi(String ssid, String pass)
{
  //KH, from v1.0.10.
  // Add option if didn't input/update SSID/PW => Use the previous saved Credentials.
  // But update the Static/DHCP options if changed.
  //if ( (ssid != "") || ( (ssid == "") && (WiFi.SSID() != "") ) )
  if ( (ssid != "") || ( (ssid == "") && WiFi.SSID() ) )
  {   
    //fix for auto connect racing issue. Move up from v1.1.0 to avoid resetSettings()
    if (WiFi.status() == WL_CONNECTED)
    {
      WIO_LOGWARN(F("Already connected. Bailing out."));
      return WL_CONNECTED;
    }
  
    if (ssid != "")
      resetSettings();

    WiFi.mode(WIFI_AP_STA); //It will start in station mode if it was previously in AP mode.

    // Hostname not working in WIO Terminal
    //setHostname();
    
    // KH, staticIP not working in WIO Terminal
    //setWifiStaticIP();

    if (ssid != "")
    {
      // Start Wifi with new values.
      WIO_LOGWARN(F("Connect to new WiFi using new IP parameters"));
      
      WiFi.begin(ssid.c_str(), pass.c_str());
    }
    else
    {
      // Start Wifi with old values.
      WIO_LOGWARN(F("Connect to previous WiFi using new IP parameters"));
      
      WiFi.begin();
    }
  }
  else if (WiFi.SSID()) 
    {
      // Start Wifi with old values.
      WIO_LOGWARN(F("Connect to previous WiFi using new IP parameters"));

      wifi_disconnect();

      WiFi.begin();
    } 
  else //if (WiFi.SSID() == "")
  {
    WIO_LOGWARN(F("No saved credentials"));
  }

  int connRes = waitForConnectResult();
  WIO_LOGWARN1("Connection result: ", getStatus(connRes));

  //not connected, WPS enabled, no pass - first attempt
  if (_tryWPS && connRes != WL_CONNECTED && pass == "")
  {
    startWPS();
    //should be connected at the end of WPS
    connRes = waitForConnectResult();
  }

  return connRes;
}

///////////////////////////////////////////////////

uint8_t WIO_WiFiManager::waitForConnectResult() 
{
  if (_connectTimeout == 0) 
  {
    return WiFi.waitForConnectResult();
  } 
  else 
  {
    WIO_LOGDEBUG(F("Waiting for connection result with time out"));
    
    unsigned long start = millis();
    bool keepConnecting = true;
    uint8_t status;
    
    while (keepConnecting) 
    {
      status = WiFi.status();
      
      if (millis() > start + _connectTimeout) 
      {
        keepConnecting = false;
        WIO_LOGDEBUG(F("Connection timed out"));
      }
      
      if (status == WL_CONNECTED || status == WL_CONNECT_FAILED) 
      {
        keepConnecting = false;
      }
      
      delay(20);
    }
    return status;
  }
}

///////////////////////////////////////////////////

void WIO_WiFiManager::startWPS() 
{
  // TODO
  WIO_LOGDEBUG("ESP32 WPS TODO");
}

///////////////////////////////////////////////////

//Convenient for debugging but wasteful of program space.
//Remove if short of space
const char* WIO_WiFiManager::getStatus(int status)
{
  switch (status)
  {
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
    default:
      return "UNKNOWN";
  }
}

///////////////////////////////////////////////////

String WIO_WiFiManager::getConfigPortalSSID() 
{
  return _apName;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::resetSettings() 
{
  WIO_LOGDEBUG(F("Removing Wi-Fi Settings"));
  // WIO_LOGDEBUG(F("THIS MAY CAUSE AP NOT TO START UP PROPERLY. YOU NEED TO COMMENT IT OUT AFTER ERASING THE DATA."));
  // TODO On ESP32 this does not erase the SSID and password. See
  // https://github.com/espressif/arduino-esp32/issues/400
  // For now, use "make erase_flash".
  WiFi.disconnect(true);
#if defined(WIO_TERMINAL)
  WiFi.clearConnectedSetting();
#endif
  delay(200);
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setTimeout(unsigned long seconds) 
{
  setConfigPortalTimeout(seconds);
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setConfigPortalTimeout(unsigned long seconds) 
{
  _configPortalTimeout = seconds * 1000;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setConnectTimeout(unsigned long seconds) 
{
  _connectTimeout = seconds * 1000;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setDebugOutput(bool debug) 
{
  _debug = debug;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn) 
{
  _ap_static_ip = ip;
  _ap_static_gw = gw;
  _ap_static_sn = sn;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn) 
{
  _sta_static_ip = ip;
  _sta_static_gw = gw;
  _sta_static_sn = sn;
}

///////////////////////////////////////////////////

#if USE_CONFIGURABLE_DNS
void WIO_WiFiManager::setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn, IPAddress dns_address_1, IPAddress dns_address_2)
{
  WIO_LOGINFO(F("setSTAStaticIPConfig for USE_CONFIGURABLE_DNS"));
  _sta_static_ip = ip;
  _sta_static_gw = gw;
  _sta_static_sn = sn;
  _sta_static_dns1 = dns_address_1; //***** Added argument *****
  _sta_static_dns2 = dns_address_2; //***** Added argument *****
}
#endif

///////////////////////////////////////////////////


void WIO_WiFiManager::setMinimumSignalQuality(int quality) 
{
  _minimumQuality = quality;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::setBreakAfterConfig(bool shouldBreak) 
{
  _shouldBreakAfterConfig = shouldBreak;
}

///////////////////////////////////////////////////

void WIO_WiFiManager::reportStatus(String &page)
{
  //page += FPSTR(WM_HTTP_SCRIPT_NTP_MSG);

  if (WiFi.SSID() != "")
  {
    page += F("Configured to connect to AP <b>");
    // Garbage with WIO Terminal
    //page += String(WiFi.SSID());

    if (WiFi.status() == WL_CONNECTED)
    {
      page += F(" and currently connected</b> on IP <a href=\"http://");
      page += WiFi.localIP().toString();
      page += F("/\">");
      page += WiFi.localIP().toString();
      page += F("</a>");
    }
    else
    {
      page += F(" but not currently connected</b> to network.");
    }
  }
  else
  {
    page += F("No network currently configured.");
  }
}

///////////////////////////////////////////////////

/** Handle root or redirect to captive portal */
void WIO_WiFiManager::handleRoot() 
{
  WIO_LOGDEBUG(F("Handle root"));
  
  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;		//KH
  
  if (captivePortal()) 
  { 
  // If caprive portal redirect instead of displaying the page.
    return;
  }

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // New from v1.1.1, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
    
  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Options");
  page += FPSTR(WM_HTTP_SCRIPT);
  //page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += "<h2>";
  page += _apName;
  page += "</h2>";
  page += F("<h3>WIO_WiFiManager</h3>");
  page += FPSTR(WM_HTTP_PORTAL_OPTIONS);
  page += F("<div class=\"msg\">");
  reportStatus(page);
  page += F("</div>");
  page += FPSTR(WM_HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}

//////////////////////////////////////////////////

/** Wifi config page handler */
void WIO_WiFiManager::handleWifi(bool scan) 
{
  WIO_LOGDEBUG(F("Handle Wifi"));
  
  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;		//KH
  
  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // New from v1.1.1, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
    
  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  
  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Config ESP");
  page += FPSTR(WM_HTTP_SCRIPT);
  //page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<h2>Configuration</h2>");

  //  KH, New, v1.0.6+
  numberOfNetworks = scanWifiNetworks(&networkIndices);

  //Print list of WiFi networks that were found in earlier scan
  if (numberOfNetworks == 0)
  {
    page += F("WiFi scan found no networks. Restart configuration portal to scan again.");
  }
  else
  {
    // From v1.0.10
    page += FPSTR(WM_FLDSET_START);
    //////
    
    //display networks in page
    for (int i = 0; i < numberOfNetworks; i++)
    {
      if (networkIndices[i] == -1)
        continue; // skip dups and those that are below the required quality
      
      WIO_LOGDEBUG1(F("Index ="), i);
      WIO_LOGDEBUG1(F("SSID ="), WiFi.SSID(networkIndices[i]));
      WIO_LOGDEBUG1(F("RSSI ="), WiFi.RSSI(networkIndices[i]));

      int quality = getRSSIasQuality(WiFi.RSSI(networkIndices[i]));

      String item = FPSTR(WM_HTTP_ITEM);
      String rssiQ;
      rssiQ += quality;
      item.replace("{v}", WiFi.SSID(networkIndices[i]));
      item.replace("{r}", rssiQ);

      if (WiFi.encryptionType(networkIndices[i]) != WIFI_AUTH_OPEN)
      {
        item.replace("{i}", "l");
      }
      else
      {
        item.replace("{i}", "");
      }

       page += item;
       delay(0);
    }
    
    // From v1.0.10
    page += FPSTR(WM_FLDSET_END);
    //////

    page += "<br/>";
  }

  page += FPSTR(WM_HTTP_FORM_START);
  char parLength[2];

  // add the extra parameters to the form
  for (int i = 0; i < _paramsCount; i++)
  {
    if (_params[i] == NULL)
    {
      break;
    }
    
    // From v1.0.10
    if (i == 1)
    {
      page += FPSTR(WM_FLDSET_START);
    }
    //////

    String pitem;
    switch (_params[i]->getLabelPlacement())
    {
      case WFM_LABEL_BEFORE:
        pitem = FPSTR(WM_HTTP_FORM_LABEL_BEFORE);
        //pitem = FPSTR(WM_HTTP_FORM_LABEL);
        //pitem += FPSTR(WM_HTTP_FORM_PARAM);
        break;
      case WFM_LABEL_AFTER:
        pitem = FPSTR(WM_HTTP_FORM_LABEL_AFTER);
        //pitem = FPSTR(WM_HTTP_FORM_PARAM);
        //pitem += FPSTR(WM_HTTP_FORM_LABEL);
        break;
      default:
        // WFM_NO_LABEL
        pitem = FPSTR(WM_HTTP_FORM_PARAM);
        break;
    }

    if (_params[i]->getID() != NULL)
    {
      pitem.replace("{i}", _params[i]->getID());
      pitem.replace("{n}", _params[i]->getID());
      pitem.replace("{p}", _params[i]->getPlaceholder());
      snprintf(parLength, 2, "%d", _params[i]->getValueLength());
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", _params[i]->getValue());
      pitem.replace("{c}", _params[i]->getCustomHTML());
    }
    else
    {
      pitem = _params[i]->getCustomHTML();
    }

    page += pitem;
  }
  
  // From v1.0.10
  if (_paramsCount > 0)
  {
    page += FPSTR(WM_FLDSET_END);
  }
  //////

  if (_params[0] != NULL)
  {
    page += "<br/>";
  }

  //WIO_LOGDEBUG1(F("Static IP ="), _sta_static_ip.toString());
  
  // KH, Comment out in v1.0.9 to permit changing from DHCP to static IP, or vice versa
  // and add staticIP label in CP
  
  // From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
  // You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
  // You have to explicitly specify false to disable the feature.

  page += FPSTR(WM_HTTP_FORM_END);

  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  WIO_LOGDEBUG(F("Sent config page")); 
}

///////////////////////////////////////////////////

/** Handle the WLAN save form and redirect to WLAN config page again */
void WIO_WiFiManager::handleWifiSave()
{
  WIO_LOGDEBUG(F("WiFi save"));

  //SAVE/connect here
  _ssid = server->arg("s").c_str();
  _pass = server->arg("p").c_str();
  
#if (!defined(USING_ONE_WIFI) || !USING_ONE_WIFI)     
  // New from v1.1.0
  _ssid1 = server->arg("s1").c_str();
  _pass1 = server->arg("p1").c_str();
  //////
#endif

  //parameters
  for (int i = 0; i < _paramsCount; i++)
  {
    if (_params[i] == NULL)
    {
      break;
    }

    //read parameter
    String value = server->arg(_params[i]->getID()).c_str();
    //store it in array
    value.toCharArray(_params[i]->_value, _params[i]->_length);
    
    WIO_LOGDEBUG2(F("Parameter and value :"), _params[i]->getID(), value);
  }

  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += FPSTR(WM_HTTP_SAVED);
  page.replace("{v}", _apName);
  page.replace("{x}", _ssid);
  
#if (!defined(USING_ONE_WIFI) || !USING_ONE_WIFI)     
  // KH, update from v1.1.0
  page.replace("{x1}", _ssid1);
  //////
#endif
  
  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  WIO_LOGDEBUG(F("Sent wifi save page"));

  connect = true; //signal ready to connect/reset

  // Restore when Press Save WiFi
  _configPortalTimeout = DEFAULT_PORTAL_TIMEOUT;
}

///////////////////////////////////////////////////

/** Handle shut down the server page */
void WIO_WiFiManager::handleServerClose()
{
  WIO_LOGDEBUG(F("Server Close"));
  
  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // New from v1.1.1, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
    
  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  
  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Close Server");
  page += FPSTR(WM_HTTP_SCRIPT);
  //page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<div class=\"msg\">");
  page += F("My network is <b>");
  page += WiFi.SSID();
  page += F("</b><br>");
  page += F("IP address is <b>");
  page += WiFi.localIP().toString();
  page += F("</b><br><br>");
  page += F("Portal closed...<br><br>");
  //page += F("Push button on device to restart configuration server!");
  page += FPSTR(WM_HTTP_END);
  server->send(200, "text/html", page);
  stopConfigPortal = true; //signal ready to shutdown config portal
    
  WIO_LOGDEBUG(F("Sent server close page"));

  // Restore when Press Save WiFi
  _configPortalTimeout = DEFAULT_PORTAL_TIMEOUT;
}

///////////////////////////////////////////////////

/** Handle the info page */
void WIO_WiFiManager::handleInfo() 
{
  WIO_LOGDEBUG(F("Info"));

// Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;		//KH

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // New from v1.1.1, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
    
  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  
  String page = FPSTR(WM_HTTP_HEAD_START);
  
  page.replace("{v}", "Info");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<h2>WiFi Information</h2>");
  reportStatus(page);
  page += F("<h3>Device Data</h3>");
  page += F("<table class=\"table\">");
  page += F("<thead><tr><th>Name</th><th>Value</th></tr></thead><tbody><tr><td>Chip ID</td><td>");
  page += WioTerminalID();

  page += F("</td></tr>");
  page += F("<tr><td>Flash Chip ID</td><td>");

  // TODO
  page += F("TODO");

  page += F("</td></tr>");
  page += F("<tr><td>IDE Flash Size</td><td>");
  page += F(" 512Kbytes</td></tr>");
  page += F("<tr><td>Real Flash Size</td><td>");

  // TODO
  page += F("TODO");

  page += F(" bytes</td></tr>");
  page += F("<tr><td>Soft AP IP</td><td>");
  page += WiFi.softAPIP().toString();
  page += F("</td></tr>");
  page += F("<tr><td>Soft AP MAC</td><td>");
  page += WiFi.softAPmacAddress();
  page += F("</td></tr>");

  page += F("<tr><td>Station IP</td><td>");
  page += WiFi.localIP().toString();
  page += F("</td></tr>");

  page += F("<tr><td>Station MAC</td><td>");
  page += WiFi.macAddress();
  page += F("</td></tr>");
  page += F("</tbody></table>");

  page += FPSTR(WM_HTTP_AVAILABLE_PAGES);

  page += F("<p/>More information about WIO_WiFiManager at");
  page += F("<p/><a href=\"https://github.com/khoih-prog/WIO_WiFiManager\">https://github.com/khoih-prog/WIO_WiFiManager</a>");
  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  WIO_LOGDEBUG(F("Sent info page"));
}

///////////////////////////////////////////////////

/** Handle the reset page */
void WIO_WiFiManager::handleReset() 
{
  WIO_LOGDEBUG(F("Reset"));
  
  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));   
  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Info");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("Module will reset in a few seconds.");
  page += FPSTR(WM_HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  WIO_LOGDEBUG(F("Sent reset page"));
  delay(5000);

  NVIC_SystemReset();

  delay(2000);
}

///////////////////////////////////////////////////

void WIO_WiFiManager::handleNotFound() 
{
  if (captivePortal()) 
  { 
    // If captive portal redirect instead of displaying the error page.
    return;
  }
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += ( server->method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for ( uint8_t i = 0; i < server->args(); i++ ) 
  {
    message += " " + server->argName ( i ) + ": " + server->arg ( i ) + "\n";
  }
  
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->sendHeader("Content-Length", String(message.length()));
  server->send ( 404, "text/plain", message );
}

///////////////////////////////////////////////////

/**
   HTTPD redirector
   Redirect to captive portal if we got a request for another domain.
   Return true in that case so the page handler do not try to handle the request again.
*/
bool WIO_WiFiManager::captivePortal() 
{
  if (!isIp(server->hostHeader()) ) 
  {
    WIO_LOGDEBUG(F("Request redirected to captive portal"));
    server->sendHeader(F("Location"), (String)F("http://") + toStringIp(server->client().localIP()), true);
    server->send(302, FPSTR(WM_HTTP_HEAD_CT2), ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server->client().stop(); // Stop is needed because we sent no content length
    
    return true;
  }
  return false;
}

///////////////////////////////////////////////////

//start up config portal callback
void WIO_WiFiManager::setAPCallback( void (*func)(WIO_WiFiManager* myWIO_WiFiManager) ) 
{
  _apcallback = func;
}

///////////////////////////////////////////////////

//start up save config callback
void WIO_WiFiManager::setSaveConfigCallback( void (*func)(void) ) 
{
  _savecallback = func;
}

//sets a custom element to add to head, like a new style tag
void WIO_WiFiManager::setCustomHeadElement(const char* element) 
{
  _customHeadElement = element;
}

///////////////////////////////////////////////////

//if this is true, remove duplicated Access Points - defaut true
void WIO_WiFiManager::setRemoveDuplicateAPs(bool removeDuplicates) 
{
  _removeDuplicateAPs = removeDuplicates;
}

///////////////////////////////////////////////////

//Scan for WiFiNetworks in range and sort by signal strength
//space for indices array allocated on the heap and should be freed when no longer required
int WIO_WiFiManager::scanWifiNetworks(int **indicesptr)
{
  WIO_LOGDEBUG(F("Scanning Network"));

  int n = WiFi.scanNetworks();

  WIO_LOGDEBUG1(F("scanWifiNetworks: Done, Scanned Networks n ="), n); 

  //KH, Terrible bug here. WiFi.scanNetworks() returns n < 0 => malloc( negative == very big ) => crash!!!
  //In .../esp32/libraries/WiFi/src/WiFiType.h
  //#define WIFI_SCAN_RUNNING   (-1)
  //#define WIFI_SCAN_FAILED    (-2)
  //if (n == 0)
  if (n <= 0)
  {
    WIO_LOGDEBUG(F("No network found"));
    return (0);
  }
  else
  {
    // Allocate space off the heap for indices array.
    // This space should be freed when no longer required.
    int* indices = (int *)malloc(n * sizeof(int));

    if (indices == NULL)
    {
      WIO_LOGDEBUG(F("ERROR: Out of memory"));
      *indicesptr = NULL;
      return (0);
    }

    *indicesptr = indices;
   
    //sort networks
    for (int i = 0; i < n; i++)
    {
      indices[i] = i;
    }

    WIO_LOGDEBUG(F("Sorting"));

    // RSSI SORT
    // old sort
    for (int i = 0; i < n; i++)
    {
      for (int j = i + 1; j < n; j++)
      {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
        {
          std::swap(indices[i], indices[j]);
        }
      }
    }

    WIO_LOGDEBUG(F("Removing Dup"));

    // remove duplicates ( must be RSSI sorted )
    if (_removeDuplicateAPs)
    {
      String cssid;
      for (int i = 0; i < n; i++)
      {
        if (indices[i] == -1)
          continue;

        cssid = WiFi.SSID(indices[i]);
        for (int j = i + 1; j < n; j++)
        {
          if (cssid == WiFi.SSID(indices[j]))
          {
            WIO_LOGDEBUG1("DUP AP:", WiFi.SSID(indices[j]));
            indices[j] = -1; // set dup aps to index -1
          }
        }
      }
    }

    for (int i = 0; i < n; i++)
    {
      if (indices[i] == -1)
        continue; // skip dups

      int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

      if (!(_minimumQuality == -1 || _minimumQuality < quality))
      {
        indices[i] = -1;
        WIO_LOGDEBUG(F("Skipping low quality"));
      }
    }

#if (DEBUG_WIFIMGR > 2)
    for (int i = 0; i < n; i++)
    {
      if (indices[i] == -1)
        continue; // skip dups
      else
        Serial.println(WiFi.SSID(indices[i]));
    }
#endif

    return (n);
  }
}

///////////////////////////////////////////////////

int WIO_WiFiManager::getRSSIasQuality(int RSSI) 
{
  int quality = 0;

  if (RSSI <= -100) 
  {
    quality = 0;
  } 
  else if (RSSI >= -50) 
  {
    quality = 100;
  } 
  else 
  {
    quality = 2 * (RSSI + 100);
  }
  
  return quality;
}

///////////////////////////////////////////////////

/** Is this an IP? */
bool WIO_WiFiManager::isIp(String str) 
{
  for (int i = 0; i < str.length(); i++) 
  {
    int c = str.charAt(i);
    
    if (c != '.' && (c < '0' || c > '9')) 
    {
      return false;
    }
  }
  
  return true;
}

///////////////////////////////////////////////////

/** IP to String? */
String WIO_WiFiManager::toStringIp(IPAddress ip) 
{
  String res = "";
  
  for (int i = 0; i < 3; i++) 
  {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  
  res += String(((ip >> 8 * 3)) & 0xFF);
  
  return res;
}

///////////////////////////////////////////////////

// We can't use WiFi.SSID() in ESP32 as it's only valid after connected.
// SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
// Have to create a new function to store in EEPROM/SPIFFS for this purpose
// Not OK in WIO now. Temporarily disable and return NULL string

String WIO_WiFiManager::getStoredWiFiSSID()
{
  if (WiFi.getMode() == WIFI_MODE_NULL)
  {
    return String();
  }

#if 0
  wifi_ap_record_t info;

  if (!esp_wifi_sta_get_ap_info(&info))
  {
    return String(reinterpret_cast<char*>(info.ssid));
  }
  else
  {
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    return String(reinterpret_cast<char*>(conf.sta.ssid));
  }
#endif

  return String();
}

///////////////////////////////////////////////////

// Not OK in WIO now. Temporarily disable and return NULL string
String WIO_WiFiManager::getStoredWiFiPass()
{
  if (WiFi.getMode() == WIFI_MODE_NULL)
  {
    return String();
  }

#if 1
  return String();
#else
  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf);
  
  return String(reinterpret_cast<char*>(conf.sta.password));
#endif
  
}

///////////////////////////////////////////////////

String WIO_WiFiManager::WioTerminalID()
{
    char ID[50];
    uint32_t *id_word0 = (uint32_t *)DEVICE_ID_WORD0;
    uint32_t *id_word1 = (uint32_t *)DEVICE_ID_WORD1;
    uint32_t *id_word2 = (uint32_t *)DEVICE_ID_WORD2;
    uint32_t *id_word3 = (uint32_t *)DEVICE_ID_WORD3;

    snprintf(ID, 30, "%02X:%02X:%02X:%02X:%02X:%02X",
        (*id_word0 >> 8) & 0xFF,
        (*id_word0 >> 0) & 0xFF,
        (*id_word3 >> 24)& 0xFF,
        (*id_word3 >> 16)& 0xFF,
        (*id_word3 >> 8) & 0xFF,
        (*id_word3 >> 0) & 0xFF
    );
    
    return ID;
}
