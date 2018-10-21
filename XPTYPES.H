/***********************************************************************************
 *  File:   XPTypes.h
 ***********************************************************************************
 *  Copyright:  © 1995 by PointCast, Inc.  All rights reserved.
 ***********************************************************************************
 *  Written by: Randy Moore
 *  Date:   09/28/95
 *
 *  Contains: Typedefs that can be used in cross platform code safely and some
 *        compatibility things between the platforms
 *
 ***********************************************************************************
 *  MODIFICATION HISTORY
 *  =====================
 *  09/28/95  Randy Intial Creation
 *
 ***********************************************************************************/
#ifndef __H_XPTypes_
#define __H_XPTypes_

/*****************************************************************************
 *  Setup Types and Defines that are platform specific
 *
 *  Note: please refrain from using int as it is highly platform and compiler
 *  specific
 *****************************************************************************/
#ifdef _WINDOWS

/*****************************************************************************
 *  Setup Windows Specific  Types
 *      Windows   | common
 *      -----------------------
 *      DWORD   | UInt32
 *      WORD    | SInt16
 *      BYTE    | UInt8
 *****************************************************************************/
typedef unsigned char UInt8;
typedef signed char   SInt8;

typedef unsigned short  UInt16;
typedef signed short  SInt16;

typedef unsigned long UInt32;
typedef signed long   SInt32;

typedef UInt16      UniChar;
typedef BOOL      Boolean;

#undef EXPORT
#if defined(WIN32) || defined(NT)
#define EXPORT _declspec( dllexport )
#define huge
#else
#define EXPORT  _export
#endif

#define TRACEEND
//#define TRACEBOLD TRACE

#ifndef UInt
  #define UInt unsigned int
#endif

#ifndef INT
  #define INT  int
#endif

#ifndef _TIME_T_DEFINED
typedef UInt32    time_t;
#define _TIME_T_DEFINED
#endif

#else // MACINTOSH
/*****************************************************************************
 *  Setup Macintosh Specific  Types
 *****************************************************************************/
#include <Types.h>
#define	MAC
#define	MACINTOSH

typedef const char *LPCSTR;
typedef const char *LPCTSTR;
typedef char *LPSTR;
typedef char *LPTSTR;
typedef Handle HGLOBAL;

#define FAR
#define near
#define huge
#define _MAX_PATH 600
#define afx_msg

#undef EXPORT
#define EXPORT

#if _DEBUG
  #include "assert.h"
  #include "UDebugging.h"

  #undef  TRACE
#if LLDEBUG
  #include "MBPrintf.h"
  #define TRACE MBPrintf
  #define TRACEBOLD MBPrintf
  #define TRACEEND  ";g"
#else
  #include "pprint.h"
  #define TRACE pprint
  #define TRACEBOLD pprintBig
  #define TRACEEND
#endif
  #define TRACE0  TRACE
  #define TRACE1  TRACE
  #define TRACE2  TRACE
  #define TRACE3  TRACE

  #if LLDEBUG
  //  #define ASSERT(x) assert(x)
    #define ASSERT(x) { if (!(x))  (void)MBPrintf(__FILE__ ":" _STR(__LINE__) " " #x " -- Failed\n"); }
    #define VERIFY(x)	 ASSERT(x)
  #else
  //For Debug
  //  #define ASSERT(x) { if (!(x))  (void)TRACEBOLD(__FILE__ ":" _STR(__LINE__) " " #x " -- Failed\n"); }
  //  #define ASSERT(x) Assert_((x))
  //  #define VERIFY(x)	 ASSERT(x)
//FIXME - disable all assert handling until we fix the DLL/thread issue
    #define ASSERT(x) ((void)0)
    #define VERIFY(x) ((void)(x))
  #endif

#else

  #undef  TRACE
  #define TRACE
  #define TRACE0
  #define TRACE1
  #define TRACE2
  #define TRACE3
  #define TRACEEND

  #define ASSERT(x) ((void)0)
  #define VERIFY(x) ((void)(x))
#endif

//  For those silly MFC 1.52 users
#define TRY	try
#define CATCH(x,y)	catch(x y)
#define END_CATCH	

#include "CObject.h"
#include "CString.h"

#define CObList     CVoidPtrList
#define CStringList   CVStringList

#define hmemcpy memcpy

#define UInt UInt32
#define INT  SInt32

#endif

/*****************************************************************************
 *  Types common to both
 *****************************************************************************/
typedef UInt8 huge *HPUInt8;

#endif //__H_XPTypes_
