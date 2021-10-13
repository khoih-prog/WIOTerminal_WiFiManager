/****************************************************************************************************************************
  WIOTerminal_WiFiManager_Debug.h
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

#ifdef WIO_WIFIMGR_DEBUG_PORT
  #define WIO_DBG_PORT      WIO_WIFIMGR_DEBUG_PORT
#else
  #define WIO_DBG_PORT      Serial
#endif

// Change _WIO_WIFIMGR_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _WIO_WIFIMGR_LOGLEVEL_
  #define _WIO_WIFIMGR_LOGLEVEL_       0
#endif

#define WIO_LOGERROR(x)         if(_WIO_WIFIMGR_LOGLEVEL_>0) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.println(x); }
#define WIO_LOGERROR0(x)        if(_WIO_WIFIMGR_LOGLEVEL_>0) { WIO_DBG_PORT.print(x); }
#define WIO_LOGERROR1(x,y)      if(_WIO_WIFIMGR_LOGLEVEL_>0) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(y); }
#define WIO_LOGERROR2(x,y,z)    if(_WIO_WIFIMGR_LOGLEVEL_>0) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(z); }
#define WIO_LOGERROR3(x,y,z,w)  if(_WIO_WIFIMGR_LOGLEVEL_>0) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(z); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(w); }

#define WIO_LOGWARN(x)          if(_WIO_WIFIMGR_LOGLEVEL_>1) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.println(x); }
#define WIO_LOGWARN0(x)         if(_WIO_WIFIMGR_LOGLEVEL_>1) { WIO_DBG_PORT.print(x); }
#define WIO_LOGWARN1(x,y)       if(_WIO_WIFIMGR_LOGLEVEL_>1) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(y); }
#define WIO_LOGWARN2(x,y,z)     if(_WIO_WIFIMGR_LOGLEVEL_>1) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(z); }
#define WIO_LOGWARN3(x,y,z,w)   if(_WIO_WIFIMGR_LOGLEVEL_>1) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(z); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(w); }

#define WIO_LOGINFO(x)          if(_WIO_WIFIMGR_LOGLEVEL_>2) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.println(x); }
#define WIO_LOGINFO0(x)         if(_WIO_WIFIMGR_LOGLEVEL_>2) { WIO_DBG_PORT.print(x); }
#define WIO_LOGINFO1(x,y)       if(_WIO_WIFIMGR_LOGLEVEL_>2) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(y); }
#define WIO_LOGINFO2(x,y,z)     if(_WIO_WIFIMGR_LOGLEVEL_>2) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(z); }
#define WIO_LOGINFO3(x,y,z,w)   if(_WIO_WIFIMGR_LOGLEVEL_>2) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(z); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(w); }

#define WIO_LOGDEBUG(x)         if(_WIO_WIFIMGR_LOGLEVEL_>3) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.println(x); }
#define WIO_LOGDEBUG0(x)        if(_WIO_WIFIMGR_LOGLEVEL_>3) { WIO_DBG_PORT.print(x); }
#define WIO_LOGDEBUG1(x,y)      if(_WIO_WIFIMGR_LOGLEVEL_>3) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(y); }
#define WIO_LOGDEBUG2(x,y,z)    if(_WIO_WIFIMGR_LOGLEVEL_>3) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(z); }
#define WIO_LOGDEBUG3(x,y,z,w)  if(_WIO_WIFIMGR_LOGLEVEL_>3) { WIO_DBG_PORT.print("[WM] "); WIO_DBG_PORT.print(x); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(y); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.print(z); WIO_DBG_PORT.print(" "); WIO_DBG_PORT.println(w); }

//#endif    //ESP_WiFiManager_Debug_H
