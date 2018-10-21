#include "stdafx.h"
#define __HttpRequestParser_cpp

#include "HttpRequestParser.h"

//*****************************************************
//*****************************************************
//**                                                 **
//** CLASS: CStringParser                            **
//**                                                 **
//*****************************************************
//*****************************************************
CStringParser::CStringParser(VerbPacket* pTokenList)
{
  ASSERT(pTokenList != NULL);
  m_pTokens = NULL;

  // determine how many tokens there are
  for (m_NumTokens = 0;
       pTokenList[m_NumTokens].lpToken != NULL;
       m_NumTokens++) NULL;

  if (0 == m_NumTokens)
  {
    ASSERT(FALSE);
    return;
  }

  m_pTokens = new VerbPacket[m_NumTokens];
  if (NULL == m_pTokens)
  {
    ASSERT(FALSE);
    return;
  }

  //
  // Okay, initialize our to-be-sorted array
  //
  for (INT k = 0; k < m_NumTokens; k++)
  {
    m_pTokens[k].lpToken = pTokenList[k].lpToken;
    m_pTokens[k].Token   = pTokenList[k].Token;
  }

  // Good ole bubble sort...
  for (INT i = 0; i < m_NumTokens; i++)
  {
    for (INT j = i + 1; j < m_NumTokens; j++)
    {
      int iStatus = strcmp(m_pTokens[i].lpToken, m_pTokens[j].lpToken);
      if (iStatus > 0)   // list in ascending order
      {
        VerbPacket swap;
        swap.lpToken         = m_pTokens[j].lpToken;
        swap.Token           = m_pTokens[j].Token;

        m_pTokens[j].lpToken = m_pTokens[i].lpToken;
        m_pTokens[j].Token   = m_pTokens[i].Token;
        
        m_pTokens[i].lpToken = swap.lpToken;
        m_pTokens[i].Token   = swap.Token;
      }
    }
  }
}


UINT CStringParser::Tokenize(LPCSTR lpVerb)
{
  ASSERT(lpVerb != NULL);
  if (NULL == lpVerb)
    return TOKEN_UNDEFINED;
  if (NULL == m_pTokens)
  {
    ASSERT(FALSE);
    return TOKEN_UNDEFINED;
  }
  // TODO : Need more efficient parsing engine
  //        right now just search list serially

  for (INT i = 0; i < m_NumTokens; i++)
  {
    int iStatus = stricmp(m_pTokens[i].lpToken, lpVerb);
    if (0 == iStatus)
      return m_pTokens[i].Token;
    if (iStatus > 0)      // list in ascending order
      return TOKEN_UNDEFINED;
  }

  return TOKEN_UNDEFINED;
}

//*****************************************************
//*****************************************************
//**                                                 **
//** CLASS: CHttpRequestParser                       **
//**                                                 **
//*****************************************************
//*****************************************************
CStringParser* CHttpRequestParser::m_pPathTokens = NULL;
CStringParser* CHttpRequestParser::m_pMethodTokens = NULL;
CStringParser* CHttpRequestParser::m_pEntityHeaderTokens = NULL;

CHttpRequestParser::CHttpRequestParser()
{

  if (NULL == m_pPathTokens)
    m_pPathTokens = new CStringParser(PathVerbs);

  if (NULL == m_pMethodTokens)
    m_pMethodTokens = new CStringParser(MethodVerbs);

  if (NULL == m_pEntityHeaderTokens)
    m_pEntityHeaderTokens = new CStringParser(EntityHeaderVerbs);
}
void CHttpRequestParser::Reset()
{
  m_lstKeys.RemoveAll(); // CStringList will clear out nodes & CStrings!

  m_strTarget.Empty();
  m_TargetPort         = 0;
  m_strURI.Empty();
  m_Method = pcnMETHOD_UNKNOWN;

  m_ApplicationId      = APP_UNKNOWN;
  m_CategoryId         = 0;
  m_LastId             = 0;

  m_strRegistrationId.Empty();
  m_BrandId            = 0;
  m_NumItems           = 0;
  m_NumReferencedItems = 0;

  m_ContentLength      = 0;
  m_RouterTime         = 0;
  m_RequestType        = 0;       // long vs. short sessions
  m_ttIfModifiedSince  = 0;
  m_URIport            = 0;
  m_strURIaddress.Empty();


}

//********************************************************
//**
//** ROUTINE: GotoEndOfLine():
//** PURPOSE:
//**
//********************************************************
void CHttpRequestParser::GotoEndOfLine(LPCSTR lpHTTP, size_t& stOffset, size_t stSize)
{
  for (stOffset; stOffset < stSize; stOffset++)
  {
    if ('\n' == lpHTTP[stOffset])
    {
      stOffset++;
      return;
    }
  }
}


//*****************************************************************
//**
//** ROUTINE: GetToken():
//** PURPOSE: Extracts a verb from the HTTP message.
//**
//*****************************************************************
CString CHttpRequestParser::GetToken(LPCSTR lpHTTPrequest,
                                     size_t& stOffset,
                                     size_t stSize,
                                     char* szDelimiters)
{
  ASSERT(lpHTTPrequest);
  LPSTR lpRequest = (LPSTR)lpHTTPrequest;

  // our argument terminators are ' ' and ':'
  int iRemaining = stSize - stOffset;
  if (iRemaining <= 0)
  {
    TRACE1("GetToken() - [line <= 0] \"%s\"\n"TRACEEND,lpHTTPrequest);
    return CString("");
  }

  size_t stDelimiterLength = 0;
  if (szDelimiters != 0L)
    stDelimiterLength = strlen(szDelimiters);
  size_t i;
  for (i = stOffset; i < stSize; i++)
  {
    switch(lpRequest[i])
    {
      default:
      {
        size_t j;
        for (j = 0; j < stDelimiterLength; j++)
        {
          if (szDelimiters[j] == lpRequest[i])
            break;
        }             // check if caller supplied new delimiter...
        if (j == stDelimiterLength)
          break;
      }
      case '\0': // end-of-line is terminator too
      case '\r':
      case '\n':
        char cTemp = lpRequest[i];
        lpRequest[i] = '\0';
        size_t stStartOfString = stOffset;
        size_t stEndOfString   = i;
        CString strVerb = &lpRequest[stOffset];
        lpRequest[i] = cTemp;
        stOffset = i + 1; // get past delimiter
        if ('\r' == lpRequest[i])
        {
          if ( (i < stSize) && ('\n' == lpRequest[i+1]) )
            stOffset++;
        }
        else if ('\n' == lpRequest[i])
        {
          if ( (i < stSize) && ('\r' == lpRequest[i+1]) )
            stOffset++;
        }

        // Okay we have a verb, see if there are any '%' in it, they represent
        // encoded characters that can now be safely decoded...
        if (-1 == strVerb.Find('%') )
          return strVerb;
        
        // okay time to decode
        strVerb.Empty();
        for (size_t j = stStartOfString; j < stEndOfString; j++)
        {
          char cCharacter = lpRequest[j];
          if ('%' == cCharacter)
          {
            int iDigit0 = lpRequest[j+2] - '0';
            if (lpRequest[j+2] > '9') // was 'A'-'F' or 'a'->'f'
            {
              iDigit0 |= 0x20;
              iDigit0 -= 0x27;
            }

            int iDigit1 = lpRequest[j+1] - '0';
            if (lpRequest[j+1] > '9') // was 'A'-'F' or 'a'->'f'
            {
              iDigit1 |= 0x20;
              iDigit1 -= 0x27;
            }
            
            cCharacter =(char)( (iDigit1 << 4) | (iDigit0 & 0xf) & 0xff);
            j += 2;
          }

          strVerb = strVerb + cCharacter;
        }

        return strVerb;
    }
  }

  stOffset = i;
  return CString("");
}

const char kszApplicationName[] = "FIDO";
const double kdHTTPversion = 1.0;

BOOL CHttpRequestParser::ParseRequestLine(CForwardObject* pObj)
{
  LPCSTR lpHTTPrequest = (LPCSTR)pObj->m_pData;
  size_t stSize = pObj->m_iOffset;

  
  // our HTTP Request-Line has the following format;
  //
  //      <method><sp><Request-URI><sp><http version><crlf>

  // First get the method
  CString strMethod = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, " ");
  ASSERT(pObj->m_stParsingOffset != stSize);
  ASSERT(!strMethod.IsEmpty() );
  UINT uiToken = m_pMethodTokens->Tokenize(strMethod);
  ASSERT(uiToken != TOKEN_UNDEFINED);
  if (uiToken == TOKEN_UNDEFINED)
    return FALSE;

  m_Method = (httpMethods)uiToken;

  // stOffset should be pointing at the start of the Request-URI

  BOOL bStatus = ParseURI(pObj);
  return bStatus;

}

//********************************************************
//**
//** ROUTINE: ParseURI():
//** PURPOSE: Parses a 'fido' specific URI, extracts relevant
//**          information into our member variables.
//**
//********************************************************
BOOL CHttpRequestParser::ParseURI(CForwardObject* pObj)
{
  // Check to see if there were any search arguments
  ASSERT(m_lstKeys.IsEmpty() ); // should be empty!
  LPCSTR lpHTTPrequest = (LPCSTR)pObj->m_pData;
  size_t stSize = pObj->m_iOffset;

  #ifndef NDEBUG
  size_t stStartOffset = pObj->m_stParsingOffset; // for debugging checks
  #endif

  // <scheme>:<path>[?<arg1>+<arg2>]<sp><HTTP version><crlf>

  // Here are our paths:
  //
  //  "old" :
  //      /FIDO1/<category#>-<lastid>[?[searcharg]+[searcharg] ]
  //
  //  "new" single search arg
  //
  //      /FIDO1/<category#>-<lastid>/<search arg>
  //
  //  "latest"
  //
  //      /FIDO-1/NumItm-10/NumRef-1/<category#>-<lastId>[/<single search arg>]
  //

  // See if there's a scheme
  size_t stStartRelativePath = pObj->m_stParsingOffset;
  CString strPathInfo = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/:");
  if (':' == lpHTTPrequest[pObj->m_stParsingOffset-1] )  // It's a scheme
  {
    if (strPathInfo.CompareNoCase("http") )
      return FALSE;

    // Okay let's get the first part of the path
    strPathInfo = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/");
  }

  // We have reached the first '/', if there's a second
  // slash then we have an IP address specification
  if (!strPathInfo.IsEmpty() )
    return FALSE;

  strPathInfo = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/-");
  if (pObj->m_stParsingOffset == stSize)
    return FALSE;

  BOOL bHasIPaddress = strPathInfo.IsEmpty(); // found second '/'

  if (bHasIPaddress)
  {
    while(strPathInfo.IsEmpty())
    {
      stStartRelativePath = pObj->m_stParsingOffset-1;
      strPathInfo = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/:");
      if (pObj->m_stParsingOffset == stSize)
        return FALSE;
    }
    
    m_strTarget = strPathInfo;
    if (':' == lpHTTPrequest[pObj->m_stParsingOffset-1]) // found a :<port> specification, must have IP address
    {
      strPathInfo = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/");
      ASSERT(!strPathInfo.IsEmpty() );
      m_TargetPort = (UINT)atoi(strPathInfo);
    }
    else
      m_TargetPort = DEFAULT_IP_PORT;

    stStartRelativePath = pObj->m_stParsingOffset-1;
    strPathInfo = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/-");
  }

  if (strPathInfo.IsEmpty() )
    return FALSE;

  // We are past any IP specification and should be here:
  //
  //    /FIDO-1/12345678-00000001?cnr+adbe HTTP/1.0<crlf>
  //     ^
  //     |__ stOffset
  // 
  //    [or the new format below]
  //
  //     /FIDO-1/12345678-00000001?cnr+adbe HTTP/1.0<crlf>
  //

  INT iPos = strPathInfo.Find(kszApplicationName);
  if (-1 == iPos)
  {
    //
    // Since this isn't an application we know about, skip over
    // the Request-line and allow subsequent parsing of the the
    // entity-header
    //
    GotoEndOfLine(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
    return TRUE; // not FIDO app, probably browser code...
  }
  // Okay we found the application name, now at the end of it is the
  // application #
  CString strAppID;
  
  if ('/' == lpHTTPrequest[pObj->m_stParsingOffset - 1])
  {
    UINT iIDoffset = iPos + strlen(kszApplicationName);
    UINT iChars = strPathInfo.GetLength() - iIDoffset;
    strAppID = strPathInfo.Right(iChars);
  }
  else
    strAppID = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/");
    
  m_ApplicationId = (INT)atoi(strAppID);
  ASSERT(m_ApplicationId == APP_FIDO);
  if (m_ApplicationId != APP_FIDO)
    return FALSE;

  CString strPathArgs;
  do
  {
    strPathArgs = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "-.");
    int i = m_pPathTokens->Tokenize(strPathArgs);
    if (i == TOKEN_UNDEFINED)
      break;

    CString strArg = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize,"/");
    switch(i)
    {
      case PTH_REGISTRATIONID:
        ASSERT(!strArg.IsEmpty() );
        m_strRegistrationId = strArg;
        break;
      case PTH_BRANDID:
        m_BrandId = (UINT)atol(strArg);
        break;
      case PTH_NUM_ITEMS:
        m_NumItems = (UINT)atoi(strArg);
        break;
      case PTH_NUM_REFERENCED:
        m_NumReferencedItems = (UINT)atoi(strArg);
        break;
      case TOKEN_UNDEFINED: // not in parsing list
      default:
        break;
    }
  }
  while(!strPathArgs.IsEmpty() );
  
  // The delimiter between a 'category' and a 'lastid' used to be
  // a period (0x2e = '.'), now we're using a hyphen ('-'). We'll
  // scan for *both* for now to be backwards compatible
  
  CString strCategory = strPathArgs; // GetToken(lpHTTPrequest, stOffset, stSize, "-.");
  ASSERT(!strCategory.IsEmpty() );
  m_CategoryId = (DWORD)atol(strCategory);

  CString strLastID = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, " ?/"); // NOTE: three delimiters
  ASSERT(!strLastID.IsEmpty() );
  m_LastId = (DWORD)atol(strLastID);

  // If the category-lastID is followed by more path, then this is a
  // lone search arg we are embedding in the path info to increase
  // cache hits on proxy servers (any URL with a '?' is not cached)

  if ('/' == lpHTTPrequest[pObj->m_stParsingOffset - 1])
  {
    CString strKey = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, " ?"); // NOTE: two delimiters
    if (!strKey.IsEmpty() )
      m_lstKeys.AddTail(strKey);
  }

  if ('?' == lpHTTPrequest[pObj->m_stParsingOffset - 1])
  {
    // There are search args, separated by '+' and
    // terminated by a space.
    do
    {
      CString strKey = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "+\" "); // NOTE: three delimiters
      if ('"' == lpHTTPrequest[pObj->m_stParsingOffset - 1] ) // starting quote!
      {
        strKey = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "\""); // NOTE: one delimiter
        CString strThrowAway = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "+ ");
      }
      m_lstKeys.AddTail(strKey);
    }
    while ('+' == lpHTTPrequest[pObj->m_stParsingOffset-1]);
  }

  size_t stEndRelativeURI = pObj->m_stParsingOffset - 1;

  // Now get the HTTP version #
  CString strHTTP = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, "/");
  ASSERT(!strHTTP.IsEmpty() );
  CString strVersion = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
  ASSERT(!strVersion.IsEmpty() );
  double dVersion = atof(strVersion);
  if (dVersion != kdHTTPversion)
    return FALSE;

  // DEVELOPER'S NOTE:
  //
  //   The only reason the _SERVER build needs the m_strURI
  //   (the relative path) is for some pagent.cpp logging.
  //
  //   The _ROUTER builds need this information for redirection
  //   responses where this becomes the 'URI' and 'Location'
  //   entity header fields.

  // extract a copy of the 'relative' URI path information (includes search args)
  ASSERT(stStartRelativePath < stSize);
  ASSERT(stStartRelativePath >= stStartOffset);
  ASSERT(stEndRelativeURI < stSize);
  if (stEndRelativeURI >= stSize)
    return FALSE;

  LPSTR lpTerminator     = (LPSTR)&lpHTTPrequest[stEndRelativeURI];
  BYTE cTemp            = lpTerminator[0];
  lpTerminator[0]        = '\0';
  m_strURI               = &lpHTTPrequest[stStartRelativePath]; // include '/'
  lpTerminator[0]        = cTemp;
  return TRUE;
}

//***************************************************************************************
//**
//** ROUTINE: ParseHeaderArg():
//** PURPOSE: Parses a single line of a Entity/General/Request
//**          header.
//**
//**
//** return values:
//**
//**          HTTP__DONE     - encountered "empty" line indicating
//**                             end of header entries.
//**
//**          HTTP__ERROR    - problem in parsing this line
//**
//**          HTTP__READMORE - successfully parsed a HTTP header
//**                             argument, there's probably more.
//**
//**          HTTP__PARSEMORE - we successfully parsed the HTTP header
//**                              argument and we aren't at the end of the
//**                              buffer.
//**
//***************************************************************************************
httpStatus CHttpRequestParser::ParseEntityHeader(CForwardObject* pObj)
{
  LPCSTR lpHTTPrequest = (LPCSTR)pObj->m_pData;
  size_t stSize = pObj->m_iOffset;

  // There are three types of headers in a full request
  //
  //  o  General-Header
  //  o  Request-Header
  //  o  Entity-Header
  //

  do
  {
    // See if we have encountered an empty header line
    if ( ('\r' == lpHTTPrequest[pObj->m_stParsingOffset] ) &&
         ('\n' == lpHTTPrequest[pObj->m_stParsingOffset+1] ) )
    {
      pObj->m_stParsingOffset += 2; // get past the <crlf>
      return HTTP_DONE;
    }

    // There's something on the line in the format:
    //
    //  [verb]:[value]'\r\n'
    //
    CString strVerb = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize, ":");
    if (strVerb.IsEmpty() )
    {
      ASSERT(!strVerb.IsEmpty() );
      return HTTP_ERROR;
    }

    // Okay we have our 'verb', no figure out if it's something
    // we care about
    UINT Token = m_pEntityHeaderTokens->Tokenize(strVerb);
    if (Token == TOKEN_UNDEFINED)
      Token = EH_UNKNOWN;

    httpEntityHeaders headerToken = (httpEntityHeaders)Token;

    switch(headerToken)
    {
      case EH_URI: // URI:<http://204.30.54.95[:8080]/....>
      {
        CString strURI = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
        ASSERT(!strURI.IsEmpty() );
        if ('<' == strURI[0])
        {
          INT iIPstart = strURI.Find("//");
          if (-1 == iIPstart)
            break;
          iIPstart += 2; // get to start of ip address
            
          INT uiLength = strURI.GetLength();
          CString strIPaddress = strURI.Mid(iIPstart, (uiLength - iIPstart) );
            
          INT iIPend = strIPaddress.Find(':');
          m_URIport = 0;
          if (-1 == iIPend)
          {
            iIPend = strIPaddress.Find('/');
            m_URIport = 80; // default port
          }
            
          ASSERT(iIPend != -1);  
          m_strURIaddress = strIPaddress.Left(iIPend);
          if (m_URIport == 0)
          {
            INT iPortstart = iIPend + 1;
            INT iPortEnd   = strIPaddress.Find('/');
            ASSERT(iPortEnd != -1);
         
            CString strPort = strIPaddress.Mid(iPortstart, (iPortEnd - iPortstart));
            m_URIport = (UINT)atoi(strPort);
          }
        }
        break;
      }
      case EH_CONTENTLENGTH:
      {
        ASSERT(m_ContentLength == 0);
        CString strLength = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
        ASSERT(!strLength.IsEmpty() );
        m_ContentLength = (size_t)atol(strLength);
        break;
      }
        
       case EH_ROUTERTIME:        // old format
       {
         CString strArg = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
         ASSERT(!strArg.IsEmpty() );
         m_RouterTime = (UINT)atol(strArg);
         break;
       }
       // Entity-header arguments that are PCN specific
        case EH_PCNREQTYPE:
       {
         CString strArg = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
         ASSERT(!strArg.IsEmpty() );
         m_RequestType = atoi( (LPCSTR)strArg);
         break;
       }
       case EH_IFMODIFIED: // "If-Modified-Since"
       {
         CString strDate = GetToken(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
         // now we need to convert the date/time stamp
         m_ttIfModifiedSince = 0;// TODO: ConvertHTTPdate(strDate);
         break;
       }
       case EH_UNKNOWN: // things we don't know about...
       default:
         GotoEndOfLine(lpHTTPrequest, pObj->m_stParsingOffset, stSize);
         break;
    }// end of processing verb
  }
  while( pObj->m_stParsingOffset < stSize);

  return HTTP_READMORE;
}// end ::ParseEntityHeader()

/*

//
// routine: CreateHTTPdate():
// purpose: Create an RFC 822 date format string
//
void CHttpRequestParser::CreateHTTPdate(time_t ttDate, LPSTR lpDate)
{
  ASSERT(lpDate != NULL);
  ASSERT(ttDate != 0);
  struct tm *ptm = gmtime(&ttDate);
  ASSERT(ptm != NULL);
  if ( (ttDate != 0) && (ptm != NULL) )
  {
    sprintf(lpDate,"%s, %02u %s %4u %02u:%02u:%02u GMT",
           lpWkdays[ptm->tm_wday],
           ptm->tm_mday,
           lpMonths[ptm->tm_mon],
           (ptm->tm_year + 1900),
           ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  }
  else
    lpDate[0] = '\0';

}// end of creating a HTTP date
*/

httpMethods  CHttpRequestParser::GetMethod()        const 
{ return m_Method;        }
UINT         CHttpRequestParser::GetPort()          const 
{ return m_TargetPort;    }
DWORD        CHttpRequestParser::GetCategoryId()    const 
{ return m_CategoryId;    }
DWORD        CHttpRequestParser::GetLastId()        const 
{ return m_LastId;        }
LPCSTR       CHttpRequestParser::GetTargetAddress() const 
{ return m_strTarget;     }
UINT         CHttpRequestParser::GetApplicationId() const 
{ return m_ApplicationId; }
UINT         CHttpRequestParser::GetNumItems()      const 
{ return m_NumItems; }
void         CHttpRequestParser::SetNumItems(UINT num)
{ m_NumItems = num; }

size_t       CHttpRequestParser::GetContentLength()
{ return m_ContentLength; } 

const CStringList* CHttpRequestParser::GetSearchArgs() const
{    return &m_lstKeys;   }

//
// CForwardObject
//

CForwardObject::CForwardObject(size_t stBufferSize /* = 1024 */)
{
  m_iSize   = stBufferSize;
  m_pData   = new BYTE[m_iSize];
  Reset();
}
void CForwardObject::Reset()
{
  m_iStart  = 0;
  m_iEnd    = 0;
  m_iOffset = 0;
  m_stParsingOffset = 0;
}

CForwardObject::~CForwardObject()
{
  delete []m_pData;
}

void CForwardObject::operator=(CForwardObject& ToCopy)
{
  m_iSize  = ToCopy.m_iEnd - m_iStart;
  m_pData  = new BYTE[m_iSize];
  memcpy(m_pData, &ToCopy.m_pData[ToCopy.m_iStart], ToCopy.m_iEnd);
  Reset();
  m_iEnd   = m_iSize;
}

