
#define DEFAULT_IP_PORT 80
#define APP_FIDO  1   // "/FIDO-1"
#define APP_UNKNOWN 0 // probably generic HTTP request (browser)

#ifdef __HttpRequestParser_h
// Note: All 'VerbPacket' structures will be sorted for
//       efficient parsing.
//
#define TOKEN_UNDEFINED 0xffff

typedef struct
{
  LPCSTR lpToken;
  UINT Token;
}VerbPacket;
#endif // __HttpRequestParser_h

//
// Define our Path Information
//
typedef enum {PTH_FIDO = 1,
              PTH_NUM_ITEMS,
              PTH_NUM_REFERENCED,
              PTH_REGISTRATIONID,
              PTH_BRANDID}fidoPathInfo;

#ifdef __HttpRequestParser_cpp
VerbPacket PathVerbs[] = { 
{"NumItm", PTH_NUM_ITEMS},
{"NumRef", PTH_NUM_REFERENCED},
{"RegID", PTH_REGISTRATIONID},
{"BID",   PTH_BRANDID},
{"FIDO",  PTH_FIDO},
{NULL, 0x0} };
#endif // __HttpRequestParser_cpp

typedef enum {pcnMETHOD_GET = 1,
              pcnMETHOD_HEAD,
              pcnMETHOD_POST,
              pcnMETHOD_PUT,
              pcnMETHOD_DELETE,
              pcnMETHOD_LINK,
              pcnMETHOD_UNLINK,
              pcnMETHOD_UNKNOWN}httpMethods;

#ifdef __HttpRequestParser_cpp
VerbPacket MethodVerbs[] = { 
{"GET",     pcnMETHOD_GET},
{"HEAD",    pcnMETHOD_HEAD},
{"POST",    pcnMETHOD_POST},
{"PUT",     pcnMETHOD_PUT},
{"DELETE",  pcnMETHOD_DELETE},
{"LINK",    pcnMETHOD_LINK},
{"UNLINK",  pcnMETHOD_UNLINK},
{NULL, 0x0} };
#endif // __HttpRequestParser_cpp

typedef enum {EH_CONTENTLENGTH = 1,
              EH_CONTENTTYPE,
              EH_EXPIRES,
              EH_URI,
              EH_SERVER,
              EH_USERAGENT,
              EH_IFMODIFIED,
              EH_LASTMODIFIED,

              EH_PCNREQTYPE,
              EH_ROUTERTIME,
              // Unknown, default parsing
              EH_UNKNOWN}httpEntityHeaders;

#ifdef __HttpRequestParser_cpp
VerbPacket EntityHeaderVerbs[] = { 
{"Content-length",    EH_CONTENTLENGTH},
{"Content-type",      EH_CONTENTTYPE},
{"Expires",           EH_EXPIRES},
{"URI",               EH_URI},
{"Server",            EH_SERVER},
{"User-Agent",        EH_USERAGENT},
{"If-Modified-Since", EH_IFMODIFIED},
{"Last-Modified",     EH_LASTMODIFIED},
{"PCNReqType",        EH_PCNREQTYPE},
{"PCNrouter-time",    EH_ROUTERTIME},
{NULL, TOKEN_UNDEFINED} };
#endif // __HttpRequestParser_cpp

typedef enum {HTTP_READMORE, HTTP_PARSEMORE, HTTP_ERROR, HTTP_DONE, HTTP_EXIT} httpStatus;

