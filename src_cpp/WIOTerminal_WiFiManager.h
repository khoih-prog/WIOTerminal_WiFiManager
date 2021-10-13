/****************************************************************************************************************************
  WIOTerminal_WiFiManager.h
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

#if !defined(WIO_TERMINAL)
  #error This code is intended to run on the WIO Terminal SAMD51 platform! Please check your Tools->Board setting.
#endif

#define WIO_TERMINAL_WIFIMANAGER_VERSION    "WIOTerminal_WiFiManager v1.2.1"

#include "WIOTerminal_WiFiManager_Debug.h"

#include <rpcWiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <memory>

#define DEVICE_ID_WORD0  ((uint32_t)0x008061FC)
#define DEVICE_ID_WORD1  ((uint32_t)0x00806010)
#define DEVICE_ID_WORD2  ((uint32_t)0x00806014)
#define DEVICE_ID_WORD3  ((uint32_t)0x00806018)

#define WFM_LABEL_BEFORE 1
#define WFM_LABEL_AFTER 2
#define WFM_NO_LABEL 0

//KH
//Mofidy HTTP_HEAD to WM_HTTP_HEAD_START to avoid conflict in Arduino esp8266 core 2.6.0+
const char WM_HTTP_200[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
const char WM_HTTP_HEAD_START[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";

// KH, update from v1.0.10
const char WM_HTTP_STYLE[] PROGMEM = "<style>div{padding:2px;font-size:1em;}body,textarea,input,select{background: 0;border-radius: 0;font: 16px sans-serif;margin: 0}textarea,input,select{outline: 0;font-size: 14px;border: 1px solid #ccc;padding: 8px;width: 90%}.btn a{text-decoration: none}.container{margin: auto;width: 90%}@media(min-width:1200px){.container{margin: auto;width: 30%}}@media(min-width:768px) and (max-width:1200px){.container{margin: auto;width: 50%}}.btn,h2{font-size: 2em}h1{font-size: 3em}.btn{background: #0ae;border-radius: 4px;border: 0;color: #fff;cursor: pointer;display: inline-block;margin: 2px 0;padding: 10px 14px 11px;width: 100%}.btn:hover{background: #09d}.btn:active,.btn:focus{background: #08b}label>*{display: inline}form>*{display: block;margin-bottom: 10px}textarea:focus,input:focus,select:focus{border-color: #5ab}.msg{background: #def;border-left: 5px solid #59d;padding: 1.5em}.q{float: right;width: 64px;text-align: right}.l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;background-size: 1em}input[type='checkbox']{float: left;width: 20px}.table td{padding:.5em;text-align:left}.table tbody>:nth-child(2n-1){background:#ddd}fieldset{border-radius:0.5rem;margin:0px;}</style>";
//////

const char WM_HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";

// KH, update from v1.0.10
const char WM_HTTP_HEAD_END[] PROGMEM = "</head><body><div class=\"container\"><div style=\"text-align:left;display:inline-block;min-width:260px;\">";

const char WM_FLDSET_START[]  PROGMEM = "<fieldset>";
const char WM_FLDSET_END[]    PROGMEM = "</fieldset>";
//////

const char WM_HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\" method=\"get\"><button class=\"btn\">Configuration</button></form><br/><form action=\"/i\" method=\"get\"><button class=\"btn\">Information</button></form><br/><form action=\"/close\" method=\"get\"><button class=\"btn\">Exit Portal</button></form><br/>";
const char WM_HTTP_ITEM[] PROGMEM = "<div><a href=\"#p\" onclick=\"c(this)\">{v}</a>&nbsp;<span class=\"q {i}\">{r}%</span></div>";
const char JSON_ITEM[] PROGMEM = "{\"SSID\":\"{v}\", \"Encryption\":{i}, \"Quality\":\"{r}\"}";

#if (defined(USING_ONE_WIFI) && USING_ONE_WIFI)
#warning USING_ONE_WIFI
// KH, update from v1.0.10
const char WM_HTTP_FORM_START[] PROGMEM = "<form method=\"get\" action=\"wifisave\"><fieldset><div><label>SSID</label><input id=\"s\" name=\"s\" length=32 placeholder=\"SSID\"><div></div></div><div><label>Password</label><input id=\"p\" name=\"p\" length=64 placeholder=\"password\"><div></div></div></fieldset>";
//////

#else

// KH, update from v1.0.10
const char WM_HTTP_FORM_START[] PROGMEM = "<form method=\"get\" action=\"wifisave\"><fieldset><div><label>SSID</label><input id=\"s\" name=\"s\" length=32 placeholder=\"SSID\"><div></div></div><div><label>Password</label><input id=\"p\" name=\"p\" length=64 placeholder=\"password\"><div></div></div><div><label>SSID1</label><input id=\"s1\" name=\"s1\" length=32 placeholder=\"SSID1\"><div></div></div><div><label>Password</label><input id=\"p1\" name=\"p1\" length=64 placeholder=\"password1\"><div></div></div></fieldset>";
//////

#endif    // #if (defined(USING_ONE_WIFI) && USING_ONE_WIFI)

// KH, add from v1.0.10
const char WM_HTTP_FORM_LABEL_BEFORE[] PROGMEM = "<div><label for=\"{i}\">{p}</label><input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}><div></div></div>";
const char WM_HTTP_FORM_LABEL_AFTER[] PROGMEM = "<div><input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}><label for=\"{i}\">{p}</label><div></div></div>";
//////

const char WM_HTTP_FORM_LABEL[] PROGMEM = "<label for=\"{i}\">{p}</label>";
const char WM_HTTP_FORM_PARAM[] PROGMEM = "<input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}>";

const char WM_HTTP_FORM_END[] PROGMEM = "<button class=\"btn\" type=\"submit\">Save</button></form>";
const char WM_HTTP_SAVED[] PROGMEM = "<div class=\"msg\"><b>Credentials Saved</b><br>Connecting {v} to the {x} network.<br>Wait around 10 seconds then check <a href=\"/\">if OK.</a> <p/>The {v} AP will be restarted on the radio channel of the {x} network. You may have to manually reconnect to the {v} AP.</div>";
const char WM_HTTP_END[] PROGMEM = "</div></body></html>";

//KH, added 2019/12/15 from Tzapu Development
// http
const char WM_HTTP_HEAD_CL[]         PROGMEM = "Content-Length";
const char WM_HTTP_HEAD_CT[]         PROGMEM = "text/html";
const char WM_HTTP_HEAD_CT2[]        PROGMEM = "text/plain";
const char WM_HTTP_HEAD_CORS[]       PROGMEM = "Access-Control-Allow-Origin";
const char WM_HTTP_HEAD_CORS_ALLOW_ALL[]  PROGMEM = "*";

//KH Add repeatedly used const
const char WM_HTTP_CACHE_CONTROL[]   PROGMEM = "Cache-Control";
const char WM_HTTP_NO_STORE[]        PROGMEM = "no-cache, no-store, must-revalidate";
const char WM_HTTP_PRAGMA[]          PROGMEM = "Pragma";
const char WM_HTTP_NO_CACHE[]        PROGMEM = "no-cache";
const char WM_HTTP_EXPIRES[]         PROGMEM = "Expires";
const char WM_HTTP_CORS[]            PROGMEM = "Access-Control-Allow-Origin";
const char WM_HTTP_CORS_ALLOW_ALL[]  PROGMEM = "*";

#if USE_AVAILABLE_PAGES
const char WM_HTTP_AVAILABLE_PAGES[] PROGMEM = "<h3>Available Pages</h3><table class=\"table\"><thead><tr><th>Page</th><th>Function</th></tr></thead><tbody><tr><td><a href=\"/\">/</a></td><td>Menu page.</td></tr><tr><td><a href=\"/wifi\">/wifi</a></td><td>Show WiFi scan results and enter WiFi configuration.</td></tr><tr><td><a href=\"/wifisave\">/wifisave</a></td><td>Save WiFi configuration information and configure device. Needs variables supplied.</td></tr><tr><td><a href=\"/close\">/close</a></td><td>Close the configuration server and configuration WiFi network.</td></tr><tr><td><a href=\"/i\">/i</a></td><td>This page.</td></tr><tr><td><a href=\"/r\">/r</a></td><td>Delete WiFi configuration and reboot. ESP device will not reconnect to a network until new WiFi configuration data is entered.</td></tr><tr><td><a href=\"/state\">/state</a></td><td>Current device state in JSON format. Interface for programmatic WiFi configuration.</td></tr><tr><td><a href=\"/scan\">/scan</a></td><td>Run a WiFi scan and return results in JSON format. Interface for programmatic WiFi configuration.</td></tr></table>";
#else
const char WM_HTTP_AVAILABLE_PAGES[] PROGMEM = "";
#endif

const char WM_HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";


///////////////////////////////////////////


#define WIFI_MANAGER_MAX_PARAMS 20

// Thanks to @Amorphous for the feature and code, from v1.0.5
// (https://community.blynk.cc/t/esp-wifimanager-for-esp32-and-esp8266/42257/13)
// Form v1.0.10, enable to configure from sketch
#ifndef USE_CONFIGURABLE_DNS
  #define USE_CONFIGURABLE_DNS      false
#endif

class WIO_WMParameter
{
  public:
    WIO_WMParameter(const char *custom);
    WIO_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WIO_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    WIO_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement);

    ~WIO_WMParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    int         getLabelPlacement();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    int         _labelPlacement;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement);

    friend class WIO_WiFiManager;
};

#define USE_DYNAMIC_PARAMS				true
#define DEFAULT_PORTAL_TIMEOUT  	60000L

class WIO_WiFiManager
{
  public:
    WIO_WiFiManager();
    
    ~WIO_WiFiManager();

    bool          autoConnect();
    bool          autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    bool          startConfigPortal();
    bool          startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(bool debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    
#if USE_CONFIGURABLE_DNS
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn,
                                       IPAddress dns_address_1, IPAddress dns_address_2);
#endif
    
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WIO_WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter
    void          addParameter(WIO_WMParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(bool shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(bool removeDuplicates);
    
    //space for indices array allocated on the heap and should be freed when no longer required
    int           scanWifiNetworks(int **indicesptr);
    
    const char*   getStatus(int status);
    
    String getStoredWiFiSSID();
    String getStoredWiFiPass();
    
    String WiFi_SSID(void)
    {
      //return WiFi.SSID();
      return getStoredWiFiSSID();
    }

    String WiFi_Pass(void)
    {
      return getStoredWiFiPass();
    }
    
    // return SSID of router in STA mode got from config portal. NULL if no user's input //KH
    String getSSID() 
    {
      if (_ssid == "") 
      {
        //WIO_LOGDEBUG(F("Reading SSID"));
        // Currently WiFi.SSID() not working.
        _ssid = String(WiFi.SSID());
        //WIO_LOGDEBUG1(F("SSID: "), _ssid);
      }

      return _ssid;
    }

    String getPassword() 
    {
      if (_pass == "") 
      {
        //WIO_LOGDEBUG(F("Reading Password"));
        _pass = String(WiFi.psk());
        //WIO_LOGDEBUG1("Password: ", _pass);
      }
      
      return _pass;
    }

    // return password of router in STA mode got from config portal. NULL if no user's input //KH
    String				getPW(void) 
    {
      //return getPassword();
      return _pass;
    }
    
#if (defined(USING_ONE_WIFI) && USING_ONE_WIFI)

    #define MAX_WIFI_CREDENTIALS        1
    
    String				getSSID(uint8_t index) 
    {
      return _ssid;
    }
    
    String				getPW(uint8_t index) 
    {
      return _pass;
    }
    //////
#else

    // New from v1.1.0
    // return SSID of router in STA mode got from config portal. NULL if no user's input //KH
    String				getSSID1(void) 
    {
      return _ssid1;
    }

    // return password of router in STA mode got from config portal. NULL if no user's input //KH
    String				getPW1(void) 
    {
      return _pass1;
    }
    
    #define MAX_WIFI_CREDENTIALS        2
    
    String				getSSID(uint8_t index) 
    {
      if (index == 0)
        return _ssid;
      else if (index == 1)
        return _ssid1;
      else     
        return String("");
    }
    
    String				getPW(uint8_t index) 
    {
      if (index == 0)
        return _pass;
      else if (index == 1)
        return _pass1;
      else     
        return String("");
    }
    //////
#endif  
    
    // New from v1.1.1, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
#if USING_CORS_FEATURE
    void setCORSHeader(const char* CORSHeaders)
    {     
      _CORS_Header = CORSHeaders;

      WIO_LOGWARN1(F("Set CORS Header to : "), _CORS_Header);
    }
    
    const char* getCORSHeader(void)
    {
      return _CORS_Header;
    }
#endif   
    
    //returns the list of Parameters
    WIO_WMParameter** getParameters();
    // returns the Parameters Count
    int           getParametersCount();
   
    String        WioTerminalID();

  private:
  
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<WebServer>        server;   

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    
    String        _ssid                   = "";
    String        _pass                   = "";
    
#if (!defined(USING_ONE_WIFI) || !USING_ONE_WIFI)    
    // New from v1.1.0
    String        _ssid1  = "";
    String        _pass1  = "";
    //////
#endif
  
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;
    
    int numberOfNetworks;
    int *networkIndices;
    
    // KH, new from v1.0.10 to enable dynamic/random channel
    // default to channel 1
    #define MIN_WIFI_CHANNEL      1
    #define MAX_WIFI_CHANNEL      12    // Channel 13 is flaky, because of bad number 13 ;-)

    int _WiFiAPChannel = 1;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;
    
#if USE_CONFIGURABLE_DNS
    IPAddress     _sta_static_dns1;
    IPAddress     _sta_static_dns2;
#endif    

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    bool          _removeDuplicateAPs     = true;
    bool          _shouldBreakAfterConfig = false;
    bool          _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    int           status = WL_IDLE_STATUS;
    
    // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
#if USING_CORS_FEATURE
    const char*     _CORS_Header          = WM_HTTP_CORS_ALLOW_ALL;   //"*";
#endif   
    //////    
    
    void          setWifiStaticIP();
    
    // New v1.1.0
    int           reconnectWifi(void);
    //////
    
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(bool scan);
    void          handleWifiSave();
    void          handleServerClose();
    void          handleInfo();
    //void          handleState();
    //void          handleScan();
    void          handleReset();
    void          handleNotFound();
    //void          handle204();
    bool          captivePortal();
    bool          configPortalHasTimeout();
    
    void          reportStatus(String &page);

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    bool          isIp(String str);
    String        toStringIp(IPAddress ip);

  

    bool          connect;
    bool          stopConfigPortal = false;
    bool          _debug = true;

    void (*_apcallback)(WIO_WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    WIO_WMParameter* _params[WIFI_MANAGER_MAX_PARAMS];

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) 
    {
      return  obj->fromString(s);
    }
    
    auto optionalIPFromString(...) -> bool 
    {
      WIO_LOGDEBUG("NO fromString METHOD ON IPAddress.");
      return false;
    }
};


