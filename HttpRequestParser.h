#define __HttpRequestParser_h

#include "HttpParsingInfo.h"

class CForwardObject : public CObject
{
	DECLARE_DYNCREATE(CForwardObject)
public:
  CForwardObject(size_t stBufferSize = 1024);
  ~CForwardObject();
  void operator=(CForwardObject&);
  void Reset();

  BYTE*   m_pData;
  size_t  m_iStart; // data to xmit start
  size_t  m_iEnd;   // data to xmit end

  size_t  m_iOffset; // current end of data in buffer
  size_t  m_iSize;  // size of allocated buffer

  size_t  m_stParsingOffset; // current parsing position
};


class CStringParser
{
public:
  CStringParser(VerbPacket* pVerbs);
  UINT Tokenize(LPCSTR lpString); // returns TOKEN_UNDEFINED if not found

protected:
  VerbPacket* m_pTokens;
  INT m_NumTokens; // # tokens in this parser
};

class CHttpRequestParser
{
public:
  CHttpRequestParser();
  void Reset();

  BOOL ParseRequestLine(CForwardObject* pObj);
  BOOL ParseURI(CForwardObject* pObj);
  httpStatus ParseEntityHeader(CForwardObject* pObj);

  httpMethods  GetMethod()        const;
  UINT         GetPort()          const;
  DWORD        GetCategoryId()    const;
  DWORD        GetLastId()        const;
  LPCSTR       GetTargetAddress() const;
  UINT         GetApplicationId() const;
  UINT         GetNumItems()      const;
  void         SetNumItems(UINT num);

  // Entity Header fields
  size_t       GetContentLength(); 

  const CStringList* GetSearchArgs() const;
private:
  CString GetToken(LPCSTR lpHTTPrequest,
                   size_t& stOffset,
                   size_t stSize,
                   char* szDelimiters = NULL);
  void GotoEndOfLine(LPCSTR lpHTTP, size_t& stOffset, size_t stSize);

protected:
  CString      m_strTarget;
  UINT         m_TargetPort;
  CString      m_strURI;
  httpMethods  m_Method;

  // FIDO specific URL portions
  UINT    m_ApplicationId;
  DWORD   m_CategoryId;
  DWORD   m_LastId;

  // Search arguments
  CStringList m_lstKeys;

  // Some path information 
  CString m_strRegistrationId;
  UINT    m_BrandId;
  UINT    m_NumItems;
  UINT    m_NumReferencedItems;

  // Entity Header fields
  size_t  m_ContentLength;
  UINT    m_RouterTime;
  UINT    m_RequestType;       // long vs. short sessions
  time_t  m_ttIfModifiedSince;
  UINT    m_URIport;
  CString m_strURIaddress;

  // Parsing structures
  static CStringParser* m_pPathTokens;
  static CStringParser* m_pMethodTokens;
  static CStringParser* m_pEntityHeaderTokens;
};
