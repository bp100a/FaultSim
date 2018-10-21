#if !defined(__GenerateData_h)
#define __GenerateData_h

#if !defined(_WINSOCKAPI_)
    #include "winsock.h" // get 'SOCKET' definition
#endif

#if !defined(__CATIDS_H)
    #include "catids.h"
#endif

typedef enum {article,      // HTML file
              summary,      // HTML file
              photo,        // HTML file with picture (GIF?)
              map,          // GIF/JPEG weather map
              csv,          // historical stock data
              tdy,          // current day stock data
              bookmark,     // bookmark file
              bookmarkURL,  // URL from a bookmark file

              endMark = 0xFFFF
             }dataType;

typedef struct
{
	DataKey dkCategoryId;  // 32-bit category id
	LPCSTR lpChannelName;  // "News - National"
    LPCSTR lpTabName;
    dataType type;
    Boolean Compressed;
}CategoryMapping;

class CHttpRequestParser;

class CGenerateData
{
public:
    typedef enum {RESPONSE_GENERATED = 1,
                  RESPONSE_NOTGENERATED,    // skip faultsim processing
                  RESPONSE_ERROR}genStatus;
	genStatus GenerateResponse(CHttpRequestParser* pRequest, SOCKET hClientSocket);

protected:
    genStatus GenerateHTML     (const CHttpRequestParser* pRequest,const CategoryMapping* pCategory, CString& strHTML, UInt32 articleIndex);
    genStatus GenerateArticle  (const CHttpRequestParser* pRequest,const CategoryMapping* pCategory, void** pResponse, size_t* pSize);
    genStatus GenerateSummary  (      CHttpRequestParser* pRequest,const CategoryMapping* pCategory, void** pResponse, size_t* pSize);
    genStatus GenerateMap      (const CHttpRequestParser* pRequest,const CategoryMapping* pCategory, void** pResponse, size_t* pSize);
    genStatus GeneratePhoto    (const CHttpRequestParser* pRequest,const CategoryMapping* pCategory, void** pResponse, size_t* pSize);
    genStatus GenerateBookMark (const CHttpRequestParser* pRequest,const CategoryMapping* pCategory, void** pResponse, size_t* pSize, SOCKET hClientSocket);
    genStatus GenerateBookMarkURL (const CHttpRequestParser* pRequest,const CategoryMapping* pCategory, void** pResponse, size_t* pSize);

    genStatus CreateDSheader (const CategoryMapping* pCategory, UInt articleIndex, CString& strDSheader, UInt size);

};

#if !defined(__CATIDS_H)
	#include "catids.h"
#endif

#define COMPRESSED (TRUE)
#define UNCOMPRESSED (FALSE)

#if defined(__GenerateData_cpp)

CategoryMapping categoryNames[] = {

    // Our News

    // Compressed News
	{655617,    "News", "National"               ,article,  COMPRESSED},
	{655633,    "News", "National Summaries"     ,summary,  COMPRESSED},
	{656129,    "News", "Political"              ,article,  COMPRESSED},
	{656145,    "News", "Political Summaries"    ,summary,  COMPRESSED},
	{655873,    "News", "International"          ,article,  COMPRESSED},
	{655889,    "News", "International Summaries",summary,  COMPRESSED},
	{5243137,   "News", "Business"               ,article,  COMPRESSED},
	{5243153,   "News", "Business Summaries"     ,summary,  COMPRESSED},

    // Our weather

    // Our Sports

    // LA Times
    {0x00c80811,    "LA Times", "Daily Photo",      photo,   COMPRESSED  },
    {0x00c80810,    "LA Times", "Daily Photo",      photo,   UNCOMPRESSED},
    {0x00c80001,    "LA Times", "Front Page",       article, COMPRESSED  },
    {0x00c80000,    "LA Times", "Front Page"   ,    article, UNCOMPRESSED},
    {0x00c80100,    "LA Times", "Nation & World",   article, UNCOMPRESSED},
    {0x00c80101,    "LA Times", "Nation & World",   article, COMPRESSED  },
    {0x00c80200,    "LA Times", "State & Local",    article, UNCOMPRESSED},
    {0x00c80201,    "LA Times", "State & Local",    article, COMPRESSED  },
    {0x00c80300,    "LA Times", "Sports",           article, UNCOMPRESSED},
    {0x00c80301,    "LA Times", "Sports",           article, COMPRESSED  },


    // Boston Globe
    {0x00ca0600,    "Boston Globe", "Page One",     article, UNCOMPRESSED},
    {0x00ca0601,    "Boston Globe", "Page One",     article, COMPRESSED  },
    {0x00ca0000,    "Boston Globe", "Nation/World", article, UNCOMPRESSED},
    {0x00ca0001,    "Boston Globe", "Nation/World", article, COMPRESSED  },
    {0x00ca0100,    "Boston Globe", "Metro/Region", article, UNCOMPRESSED},
    {0x00ca0101,    "Boston Globe", "Metro/Region", article, COMPRESSED  },
    {0x00ca0200,    "Boston Globe", "Editorials",   article, UNCOMPRESSED},
    {0x00ca0201,    "Boston Globe", "Editorials",   article, COMPRESSED  },
    {0x00ca0300,    "Boston Globe", "Business",     article, UNCOMPRESSED},
    {0x00ca0301,    "Boston Globe", "Business",     article, COMPRESSED  },
    {0x00ca0400,    "Boston Globe", "Living/Arts",  article, UNCOMPRESSED},
    {0x00ca0401,    "Boston Globe", "Living/Arts",  article, COMPRESSED  },
    {0x00ca0500,    "Boston Globe", "Sports",       article, UNCOMPRESSED},
    {0x00ca0501,    "Boston Globe", "Sports",       article, COMPRESSED  },

    // Sports
    {0x00280100,    "Sports",       "News",              article, UNCOMPRESSED},
    {0x00280101,    "Sports",       "News",              article, COMPRESSED  },
    {0x00280110,    "Sports",       "News Summary",      summary, UNCOMPRESSED},
    {0x00280111,    "Sports",       "News Summary",      summary, COMPRESSED  },

    //Sports, Major League Baseball
    {0x00280200,    "Sports",       "Major League Baseball",            article, UNCOMPRESSED},
    {0x00280210,    "Sports",       "Major League Baseball Summary",    summary, UNCOMPRESSED},
    {0x00280201,    "Sports",       "Major League Baseball",            article, COMPRESSED  },
    {0x00280211,    "Sports",       "Major League Baseball Summary",    summary, COMPRESSED  },

    {0x00280300,    "Sports",       "NBA Basketball",             article, UNCOMPRESSED},
    {0x00280310,    "Sports",       "NBA Basketball Summary",     summary, UNCOMPRESSED},
    {0x00280301,    "Sports",       "NBA Basketball",             article, COMPRESSED  },
    {0x00280311,    "Sports",       "NBA Basketball Summary",     summary, COMPRESSED  },

    {0x00280400,    "Sports",       "College Basketball",         article, UNCOMPRESSED},
    {0x00280410,    "Sports",       "College Basketball Summary", summary, UNCOMPRESSED},
    {0x00280401,    "Sports",       "College Basketball",         article, COMPRESSED  },
    {0x00280411,    "Sports",       "College Basketball Summary", summary, COMPRESSED  },

    {0x00280500,    "Sports",       "Women's Basketball",         article, UNCOMPRESSED},
    {0x00280510,    "Sports",       "Women's Basketball Summary", summary, UNCOMPRESSED},
    {0x00280501,    "Sports",       "Women's Basketball",         article, COMPRESSED  },
    {0x00280511,    "Sports",       "Women's Basketball Summary", summary, COMPRESSED  },

    {0x00280600,    "Sports",       "NFL Football",               article, UNCOMPRESSED},
    {0x00280610,    "Sports",       "NFL Football Summary",       summary, UNCOMPRESSED},
    {0x00280601,    "Sports",       "NFL Football",               article, COMPRESSED  },
    {0x00280611,    "Sports",       "NFL Football Summary",       summary, COMPRESSED  },

    {0x00280700,    "Sports",       "CFL Football",               article, UNCOMPRESSED},
    {0x00280710,    "Sports",       "CFL Football Summary",       summary, UNCOMPRESSED},
    {0x00280701,    "Sports",       "CFL Football",               article, COMPRESSED  },
    {0x00280711,    "Sports",       "CFL Football Summary",       summary, COMPRESSED  },

    {0x00280800,    "Sports",       "College Football",           article, UNCOMPRESSED},
    {0x00280810,    "Sports",       "College Football Summary",   summary, UNCOMPRESSED},
    {0x00280801,    "Sports",       "College Football",           article, COMPRESSED  },
    {0x00280811,    "Sports",       "College Football Summary",   summary, COMPRESSED  },

    {0x00280900,    "Sports",       "General",                    article, UNCOMPRESSED},
    {0x00280910,    "Sports",       "General Summary",            summary, UNCOMPRESSED},
    {0x00280901,    "Sports",       "General",                    article, COMPRESSED  },
    {0x00280911,    "Sports",       "General Summary",            summary, COMPRESSED  },

    {0x00280a00,    "Sports",       "AutoRacing",                 article, UNCOMPRESSED},
    {0x00280a10,    "Sports",       "AutoRacing Summary",         summary, UNCOMPRESSED},
    {0x00280a01,    "Sports",       "AutoRacing",                 article, COMPRESSED  },
    {0x00280a11,    "Sports",       "AutoRacing Summary",         summary, COMPRESSED  },

    {0x00280b00,    "Sports",       "Major League Soccer",        article, UNCOMPRESSED},
    {0x00280b10,    "Sports",       "Major League Soccer Summary",summary, UNCOMPRESSED},
    {0x00280b01,    "Sports",       "Major League Soccer",        article, COMPRESSED  },
    {0x00280b11,    "Sports",       "Major League Soccer Summary",summary, COMPRESSED  },

    {0x00280c00,    "Sports",       "Golf",                       article, UNCOMPRESSED},
    {0x00280c10,    "Sports",       "Golf Summary",               summary, UNCOMPRESSED},
    {0x00280c01,    "Sports",       "Golf",                       article, COMPRESSED  },
    {0x00280c11,    "Sports",       "Golf Summary",               summary, COMPRESSED  },

    {0x00280d00,    "Sports",       "Tennis",                     article, UNCOMPRESSED},
    {0x00280d10,    "Sports",       "Tennis Summary",             summary, UNCOMPRESSED},
    {0x00280d01,    "Sports",       "Tennis",                     article, COMPRESSED  },
    {0x00280d11,    "Sports",       "Tennis Summary",             summary, COMPRESSED  },

    {0x00281000,    "Sports",       "Golf & Tennis",              article, UNCOMPRESSED},
    {0x00281010,    "Sports",       "Golf & Tennis Summary",      summary, UNCOMPRESSED},
    {0x00281001,    "Sports",       "Golf & Tennis",              article, COMPRESSED  },
    {0x00281011,    "Sports",       "Golf & Tennis Summary",      summary, COMPRESSED  },

    {0x00281100,    "Sports",       "NHL Hockey",                 article, UNCOMPRESSED},
    {0x00281110,    "Sports",       "NHL Hockey Summary",         summary, UNCOMPRESSED},
    {0x00281101,    "Sports",       "NHL Hockey",                 article, COMPRESSED  },
    {0x00281111,    "Sports",       "NHL Hockey Summary",         summary, COMPRESSED  },

    {0x00281200,    "Sports",       "Olympics",                   article, UNCOMPRESSED},
    {0x00281210,    "Sports",       "Olympics Summary",           summary, UNCOMPRESSED},
    {0x00281201,    "Sports",       "Olympics",                   article, COMPRESSED  },
    {0x00281211,    "Sports",       "Olympics Summary",           summary, COMPRESSED  },

    {0x00281300,    "Sports",       "System Message",             article, UNCOMPRESSED},
    {0x00281310,    "Sports",       "System Message Summary",     summary, UNCOMPRESSED},
    {0x00281301,    "Sports",       "System Message",             article, COMPRESSED  },
    {0x00281311,    "Sports",       "System Message Summary",     summary, COMPRESSED  },

    {0x001e0100,    "Weather",      "Maps",                       map,     UNCOMPRESSED},
    {0x001e0101,    "Weather",      "Maps",                       map,     COMPRESSED  },


    // This is the "PointCast" channel
    {0x00320100,    "PCN-ShowCase", "Travel",                     article, UNCOMPRESSED},
    {0x00320200,    "PCN-ShowCase", "Finance",                    article, UNCOMPRESSED},

    {0x00320300,    "PCN-Internet", "Computer",                   bookmark, UNCOMPRESSED},
    {0x00320400,    "PCN-Internet", "Entertainment",              bookmark, UNCOMPRESSED},

    {0x0032ffff,    "PCN-BookMark URL", "Synthesized URL",        bookmarkURL, UNCOMPRESSED},

    {0, NULL, NULL, article, COMPRESSED} };

#endif // end of only instantiating data for GenerateData.cpp file

#endif // end of inclusion test
