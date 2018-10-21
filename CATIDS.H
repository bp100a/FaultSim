// ----------------------------------------------------------------------------------------------------
//
// P O I N T C A S T ,   I N C O R P O R A T E D
//
// ----------------------------------------------------------------------------------------------------
//
// F I D O
//
// (C) Copyright 1995 PointCast, Incorporated.  All rights reserved.
//
// ----------------------------------------------------------------------------------------------------
//
// File:        catids.h
// Project:     fido\base\fidobase
// Author:      Harry J. Collins
//
// ----------------------------------------------------------------------------------------------------
//
// NOTICE:      All information contained herein or attendant hereto is, and remains, the
//              property of PointCast, Incporated.  Many of the intellectual and
//              technical concepts contained herein are proprietary to PointCast, Incorporated
//              and may be covered by U.S. and Foreign Patents or Patents
//              Pending, or are protected as trade secrets.  Any dissemination of this
//              information or reproduction of this material is strictly forbidden unless
//              prior written permission is obtained from PointCast, Incorporated.
//
// ----------------------------------------------------------------------------------------------------
//
// Abstract:                
//
//    'Category ids' provide a method by which the client (i.e. FIDO) can specify exactly what
//    type of data it desires. The category id is a 32 bit value (DWORD) that we have broken down
//    to present the heirarchy of information.
//
//    Note, Category ID is NO longer tied to the channel id.  Channel ids are stored in
//    chnl_ids.h
//
// ----------------------------------------------------------------------------------------------------
//

//
// all CPointCastDll's and need id's -- these are DLL i.d.'s
//
#ifndef __CATIDS_H
#define __CATIDS_H
#include "XPTypes.h"


typedef UInt32  DataKey;
typedef UInt16  CategoryID;
typedef UInt16  Subject;
typedef UInt8 Item;
typedef UInt8 Version;

const CategoryID kdwUnknown             = 0x0;
const DataKey    kdkUnknown             = 0x0L;

const CategoryID kdwDeactivated         = 0x8000; // Any negative cat id indicates an inactive article.

const CategoryID kdwCatAdmin            = 0x01;
const CategoryID kdwCatNews             = 0x0a;
const CategoryID kdwCatFinance          = 0x14;  // Company News & stock charts (personnel finance)
const CategoryID kdwCatWeather          = 0x1e;
const CategoryID kdwCatSports           = 0x28;
const CategoryID kdwCatHotSites         = 0x32;
const CategoryID kdwCatVersionCtl       = 0x3c;  // version control
const CategoryID kdwCatShadowVersionCtl = 0x3d;  // "shadow" version control
const CategoryID kdwCatWeb              = 0x46;
const CategoryID kdwCatBusiness         = 0x50;  // General Business & Industry news
const CategoryID kdwCatEntertainment    = 0x5A;  // Entertainment content
const CategoryID kdwCatTestOnly         = 0x64;  // test only
const CategoryID kdwCatHelp             = 0x6e;

// Start at 200. Only skip 2 for LMP categories.  Note, this is NOT tied to channel id!

const CategoryID kdwCatLATimes          = 0xC8;
const CategoryID kdwCatBostonGlobe      = 0xCA;
const CategoryID kdwCatTimeWarner       = 0xCC;
const CategoryID kdwCatNYTimes          = 0xCE;
const CategoryID kdwCatEDS              = 0xD0;
const CategoryID kdwCatWashPost         = 0xD2;
const CategoryID kdwCatHoustonChron     = 0xD4;
const CategoryID kdwCatCMP              = 0xD6;

const CategoryID kdwSproutMakerStatic   = 1000; //0x3E8
const CategoryID kdwSproutMakerAd       = 1010; //0x3F2

// 
// Structure of the DataKey:
//
//      31                      16   15            8    7        4    3        0
//      +------------------------+   +--------------+   +---------+   +---------+
//      |                        |   |              |   |         |   |         |
//      |  Category              | o | Subject      | o | Item    | o | Version |
//      |                        |   |              |   |         |   |         |
//      +------------------------+   +--------------+   +---------+   +---------+
//
//
//          |_ News
//          |    |_ National
//          |    |_ International
//          |    |_ Washington
//          |              ^
//          |_ Sports      |__ "subject"
//          |_ Finance
//          |_ Weather
//          |    |_ United States
//          |            |_ maps
//          |            |_ forecasts
//          |                   ^
//          |_ Hotsites         |_ "item"
//                ^
//                |_ "category"
//
//
//

typedef struct
{
  DataKey dkKey;
  LPSTR   lpSubDirectory;
  LPSTR   lpTitle;
}SubjectPacket;

// Some macros that we'll find useful in manipulating our values

#define EXTRACT_CATEGORY(a) (CategoryID)( (a & 0xFFFF0000) >> 16)
#define EXTRACT_SUBJECT(a)     (Subject)( (a & 0x0000FF00) >> 8)
#define EXTRACT_ITEM(a)           (Item)( (a & 0x000000F0) >> 4)
#define EXTRACT_VERSION(a)     (Version)( (a & 0x0000000F) )

#define MAKE_KEY(c,s,i,v)  (DataKey)( ((UInt32)c << 16L) | \
                                      ((UInt32)s <<  8L) | \
                                      ((UInt32)i <<  4L) | \
                                      ((UInt32)v) )
#endif // end of inclusion check
