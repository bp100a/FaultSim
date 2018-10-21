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
// File:        ADM_IDS.H
// Project:     FIDO
// Author:      John W. Nogrady
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
// ----------------------------------------------------------------------------------------------------
//

//
// all CPointCastDll's and need id's -- these are DLL i.d.'s
//

#ifndef __ADM_IDS_H
#define __ADM_IDS_H

#ifndef __CATIDS_H
#include "catids.h"    // fidobase, core categories...
#endif

// Now it's time to specify our Administrative specific categories
//
// Our *master* category is:
//
//      kdwCatAdmin (Administrative)
// defined in 'catids.h'

const Subject ksbjAdmRegistration   = 0x1;
const Subject ksbjAdmUploads        = 0x2;
const Subject ksbjAdmAuthentication = 0x3;

const Item kitmAdmRegistrationDefault   = 0x0;
const Item kitmAdmAuthenticationDefault = 0x0;

const Item kitmAdmUploadUnknown         = 0x0;
const Item kitmAdmUploadRegistration    = 0x1;
const Item kitmAdmUploadAdStats         = 0x2;
const Item kitmAdmUploadProfile         = 0x3;
const Item kitmAdmUploadInformation     = 0x4;

const Version kvAdmVersion                = 0x0;  // use for all
const Version kvAdmAdUploadEncode         = 0x1;
const Version kvAdmAdUploadCompressEncode = 0x2;
const Version kvAdmAdUploadTrickle        = 0x3;

const DataKey dkAdm_Registration   = MAKE_KEY(kdwCatAdmin, ksbjAdmRegistration, kitmAdmRegistrationDefault, kvAdmVersion);
const DataKey dkAdm_UploadUnkwn    = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadUnknown, kvAdmVersion);
const DataKey dkAdm_UploadReg      = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadRegistration, kvAdmVersion);
const DataKey dkAdm_UploadStats    = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadAdStats, kvAdmVersion);
const DataKey dkAdm_UploadProfile  = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadProfile, kvAdmVersion);
const DataKey dkAdm_Authentication = MAKE_KEY(kdwCatAdmin, ksbjAdmAuthentication, kitmAdmAuthenticationDefault, kvAdmVersion);
const DataKey dkAdm_UploadStatsEncode    = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadAdStats, kvAdmAdUploadEncode);
const DataKey dkAdm_UploadStatsCompressEncode = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadAdStats, kvAdmAdUploadCompressEncode);
const DataKey dkAdm_UploadStatsTrickle = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadAdStats, kvAdmAdUploadTrickle);
const DataKey dkAdm_UploadMiscInfo = MAKE_KEY(kdwCatAdmin, ksbjAdmUploads, kitmAdmUploadInformation, kvAdmVersion);

const SubjectPacket spAdminList[] = { {dkAdm_Registration,  "adm\\",  "PCN Registration"},
                                      {dkAdm_UploadUnkwn,   "adm\\",  "Administration"},
                                      {dkAdm_UploadReg,     "adm\\",  "Administration"},
                                      {dkAdm_UploadStats,   "adm\\",  "Administration"},
                                      {dkAdm_UploadProfile, "adm\\",  "Administration"},
                                      {dkAdm_Authentication,"adm\\",  "Administration"},
                           
                           // end marker...
                           {kdkUnknown, "", ""} };

#endif // end of inclusion check
