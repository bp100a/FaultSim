#include "stdafx.h"

#define __GenerateData_cpp
#if !defined(__GenerateData_h)
    #include "GenerateData.h"
#endif

#if !defined(__HttpRequestParser_h)
	#include "HttpRequestParser.h"
#endif


CGenerateData::genStatus CGenerateData::GenerateResponse(CHttpRequestParser* pRequest,
                                        SOCKET hClientSocket)
{

    ASSERT(pRequest != NULL);
    if (NULL == pRequest)
        return RESPONSE_ERROR;

    DataKey dkCategoryId = pRequest->GetCategoryId();
    if (0 == dkCategoryId)
        return RESPONSE_NOTGENERATED; // this is probably a URL (browser request)


    for (unsigned int i = 0; categoryNames[i].dkCategoryId != 0; i++)
    {
        if (categoryNames[i].dkCategoryId != dkCategoryId)
            continue;
    
        dataType type = categoryNames[i].type;
        void *pData = NULL;
        size_t size = 0;
        genStatus Status = RESPONSE_ERROR;

        switch(type)
        {
            case article:
                Status = GenerateArticle(pRequest, &categoryNames[i], &pData, &size);
                break;
            case summary:
                Status = GenerateSummary(pRequest, &categoryNames[i], &pData, &size);
                break;
            case photo:
                Status = GeneratePhoto(pRequest, &categoryNames[i], &pData, &size);
                break;
            case map:
                Status = GenerateMap(pRequest, &categoryNames[i], &pData, &size);
                break;
            case bookmark:
                Status = GenerateBookMark(pRequest, &categoryNames[i], &pData, &size, hClientSocket);
                break;
            case bookmarkURL:
                Status = GenerateBookMarkURL(pRequest, &categoryNames[i], &pData, &size);
                break;
            default:
                break;
        }

        // Okay, now we need to send back the response...
        if (RESPONSE_GENERATED == Status)
        {
            // We need a HTTP entity-header, since the 'pData' is just
            // the entity body...
            CString strContentType;
            if (type == bookmarkURL)
                strContentType.Format("text/html");
            else
                strContentType.Format("application/octect-stream");

            CString strEntityHeader;
            strEntityHeader.Format("HTTP/1.0 200 Debug - Synthesized Data\r\nContent-type: %s\r\nContent-Length:%u\r\n", strContentType, size);
            strEntityHeader += "Server:FaultSim-Synthesis\r\n";
            strEntityHeader += "\r\n";

            UInt length = strEntityHeader.GetLength();
            int sockStatus = send(hClientSocket, (LPCSTR)strEntityHeader, length,0);
            if (SOCKET_ERROR != sockStatus)
              sockStatus = send(hClientSocket, (const char *)pData, size,0);
            delete [] pData;
            if (SOCKET_ERROR == sockStatus)
                Status = RESPONSE_ERROR;
        }

        return Status;
    }// end of for loop

    return RESPONSE_NOTGENERATED;
}

CGenerateData::genStatus CGenerateData::GenerateHTML(const CHttpRequestParser* pRequest,
                                       const CategoryMapping* pCategory, 
                                       CString& strHTML,
                                       UInt32 articleIndex)
{
    // Our articles are all text so we'll compose them with CStrings
    // (this is test, make it simple)
    CString strChannelName = pCategory->lpChannelName;
    CString strTabName     = pCategory->lpTabName;

    const char kszHEAD[]      = "<HTML>\r\n<HEAD>\r\n<! -- Copyright (c) 1996 - PointCast, Inc. - All Rights Reserved>\r\n";
    const char kszPCTITLE[]   = "<META NAME=\"PCTITLE\" CONTENT=\"";
    const char kszARTICLEID[] = "<META NAME=\"PC_ARTICLE_ID\" CONTENT=";
    const char kszTAIL[]      = "</HEAD>\r\n<BODY>\r\n";

    CStringList lstArticles;

    CString strTitleName;
    strTitleName.Format("%s:%s-%u",(LPCSTR)strChannelName,(LPCSTR)strTabName, articleIndex);

    CString strTitle;
    strTitle.Format("%s%s:%s, headline #%d\">\r\n",
                    kszPCTITLE,(LPCSTR)strChannelName,
                    (LPCSTR)strTabName, articleIndex);

    CString strArticleId;
    strArticleId.Format("%s\"%s\">\r\n<TITLE>%s</TITLE>\r\n",
                        kszARTICLEID,
                        (LPCSTR)strTitleName,
                        (LPCSTR)strTitleName);
    
    time_t tt;
    time(&tt);
    struct tm *ptm = localtime(&tt);
    LPSTR lpTime = (ptm != NULL ? asctime(ptm) : NULL);

    const char kszNoTime[] = "time error";
    if (NULL == lpTime)
        lpTime = (LPSTR)kszNoTime;
    else
    {
        int len = strlen(lpTime);
        if ('\n' == lpTime[len-2])
            lpTime[len-2] = '\0';
    }

    CString strTimeStamp;
    strTimeStamp.Format("<i>%s</i><p>\r\n", lpTime);

    CString strHeadline;
    UInt numBodyLines = 4;
    strHeadline.Format("<h3>There are %u body lines for '%s'</h3>\r\n",
                        numBodyLines, (LPCSTR)strTitleName);

    // Now synthesize the data
    CString strBody;
    for (UInt j = 0; j < numBodyLines; j++)
    {
        CString strNewLine;
        strNewLine.Format("Body line %d for %s:%s, article #%d<p>",
                           j+1,
                           (LPCSTR)strChannelName,
                           (LPCSTR)strTabName,
                           articleIndex);

        strBody += strNewLine;
    }

    // Now we need to piece a complete article together
    strHTML = CString(kszHEAD) +
              strTitle         +
              strArticleId     +
              CString("</HEAD>\r\n<BODY>\r\n") +
              strTimeStamp     +
              strHeadline      +
              strBody          +
              CString("</BODY>\r\n</HTML>\r\n");

    return RESPONSE_GENERATED;
}


CGenerateData::genStatus CGenerateData::GenerateArticle(const CHttpRequestParser* pRequest,
                                       const CategoryMapping* pCategory, 
                                       void** ppResponse, size_t* pSize)
{
    ASSERT(ppResponse != NULL);

    // How many articles do we need?
    UInt NumArticles = pRequest->GetNumItems();
    if (0 == NumArticles)
        NumArticles = 1;

    // Our articles are all text so we'll compose them with CStrings
    // (this is test, make it simple)
    CString strChannelName = pCategory->lpChannelName;
    CString strTabName     = pCategory->lpTabName;

    const char kszHEAD[]      = "<HTML>\r\n<HEAD>\r\n<! -- Copyright (c) 1996 - PointCast, Inc. - All Rights Reserved>\r\n";
    const char kszPCTITLE[]   = "<META NAME=\"PCTITLE\" CONTENT=\"";
    const char kszARTICLEID[] = "<META NAME=\"PC_ARTICLE_ID\" CONTENT=";
    const char kszTAIL[]      = "</HEAD>\r\n<BODY>\r\n";

    CStringList lstArticles;
    UInt articleIndex = pRequest->GetLastId();

    for (UInt i = 0; i < NumArticles; i++)
    {
        CString strTitleName;
        strTitleName.Format("%s:%s-%u",(LPCSTR)strChannelName,(LPCSTR)strTabName, i+1);

        CString strTitle;
        strTitle.Format("%s%s:%s, headline #%d\">\r\n",
                        kszPCTITLE,(LPCSTR)strChannelName,
                        (LPCSTR)strTabName, articleIndex);

        CString strArticleId;
        strArticleId.Format("%s\"%s\">\r\n<TITLE>%s</TITLE>\r\n",
                            kszARTICLEID,
                            (LPCSTR)strTitleName,
                            (LPCSTR)strTitleName);
    
        time_t tt;
        time(&tt);
        struct tm *ptm = localtime(&tt);
        LPSTR lpTime = (ptm != NULL ? asctime(ptm) : NULL);

        const char kszNoTime[] = "time error";
        if (NULL == lpTime)
            lpTime = (LPSTR)kszNoTime;
        else
        {
            int len = strlen(lpTime);
            if ('\n' == lpTime[len-2])
                lpTime[len-2] = '\0';
        }

        CString strTimeStamp;
        strTimeStamp.Format("<i>%s</i><p>\r\n", lpTime);

        CString strHeadline;
        UInt numBodyLines = 4;
        strHeadline.Format("<h3>There are %u body lines for '%s'</h3>\r\n",
                            numBodyLines, (LPCSTR)strTitleName);

        // Now synthesize the data
        CString strBody;
        for (UInt j = 0; j < numBodyLines; j++)
        {
            CString strNewLine;
            strNewLine.Format("Body line %d for %s:%s, article #%d<p>",
                               j+1,
                               (LPCSTR)strChannelName,
                               (LPCSTR)strTabName,
                               articleIndex);

            strBody += strNewLine;
        }

        CString strDSheader;

        // Now we need to piece a complete article together
        CString strDataStream = CString(kszHEAD) +
                                strTitle         +
                                strArticleId     +
                                CString("</HEAD>\r\n<BODY>\r\n") +
                                strTimeStamp     +
                                strHeadline      +
                                strBody          +
                                CString("</BODY>\r\n</HTML>\r\n");

        UInt length = strDataStream.GetLength();
        Boolean Created = CreateDSheader(pCategory, articleIndex, strDSheader, length);
        

        CString strData = strDSheader + strDataStream;
        lstArticles.AddTail(strData);
        articleIndex++;
    }               

    // Now we have a list of 'data' streams that we need to accumulate into
    // an entity body.

    CString strEntityBody;
    while(!lstArticles.IsEmpty() )
    {
        CString strDataStream = lstArticles.RemoveHead();
        strEntityBody += strDataStream;
    }

    *pSize = strEntityBody.GetLength();
    if (0 != *pSize)
    {
        *ppResponse = new char[*pSize];
        if (*ppResponse != NULL)
        {
            memcpy(*ppResponse, (LPCSTR)strEntityBody, *pSize);
            return RESPONSE_GENERATED;
        }
    }

    return RESPONSE_ERROR;
}


CGenerateData::genStatus CGenerateData::GenerateSummary(CHttpRequestParser* pRequest,
                                       const CategoryMapping* pCategory, 
                                       void** ppResponse, size_t* pSize)
{
    ASSERT(ppResponse != NULL);
    pRequest->SetNumItems((UINT)1);
    return GenerateArticle(pRequest, pCategory, ppResponse, pSize);
}

CGenerateData::genStatus CGenerateData::GeneratePhoto(const CHttpRequestParser* pRequest,
                                     const CategoryMapping* pCategory, 
                                     void** ppResponse, size_t* pSize)
{
    ASSERT(ppResponse != NULL);
    return GenerateArticle(pRequest, pCategory, ppResponse, pSize);
}

CGenerateData::genStatus CGenerateData::GenerateMap(const CHttpRequestParser* pRequest,
                                   const CategoryMapping* pCategory, 
                                   void** ppResponse, size_t* pSize)
{
    ASSERT(ppResponse != NULL);

    // We need to get the search argument to determine what map we
    // are to create (and it's caption file)

    const CStringList* pArgs = pRequest->GetSearchArgs();
    if (NULL == pArgs)
        return RESPONSE_ERROR; // some problem with packet

    // Okay we have our arguments, time to make our maps...
    for (POSITION pos = pArgs->GetHeadPosition(); pos; NULL)
    {
        CString strMap = pArgs->GetNext(pos);
        if (strMap.IsEmpty() )
            continue; // problem...

        // We have a map name, now let's make up our 
        // caption string
        CString strCaption;
        strCaption.Format("Synthesized map '%s'", (LPCSTR)strMap);

        // Now we need to get our map, a GIF file
    }
    return RESPONSE_NOTGENERATED;
}

CGenerateData::genStatus CGenerateData::CreateDSheader(const CategoryMapping* pCategory,
                                                       UInt articleIndex,
                                                       CString& strDS,
                                                       UInt length)
{
    if (NULL == pCategory)
        return RESPONSE_ERROR;

    // A Data Stream header is as follows:
    //
    //  NXTUPD:<generated>\r\n
    //  OFF:0\r\n
    //  LEN:0\r\n
    //  LID:<last id>\r\n
    //  TLEN:<length>\r\n
    //  FP:0\r\n
    //  CMP:21\r\n
    //  SRC:<srcName>\r\n
    //  \r\n
    UInt nextUpdate = 60; // 1 minute from now
    CString srcName;
    switch(pCategory->type)
    {
    case photo:         // an HTML file referencing an image
    case article:       // an article
    case summary:       // an article that is a 'summary' of articles
        srcName.Format("%07u.htm", articleIndex);
        break;
    case map:
        srcName.Format("%07u.gif", articleIndex);
        break;
    }

    strDS.Format("NXTUPD:%u\r\nOFF:0\r\nLEN:0\r\nLID:%u\r\nTLEN:%u\r\nFP:0\r\nCMP:21\r\nSRC:%s\r\n\r\n",
                 nextUpdate, articleIndex, length, srcName);
    
    return RESPONSE_GENERATED;
}

const char kszCHANNEL[] = "PACHANNEL";
const char kszBOOKMARK[] = "PAARTICLE";

CGenerateData::genStatus CGenerateData::GenerateBookMark(const CHttpRequestParser* pRequest,
                                   const CategoryMapping* pCategory, 
                                   void** ppResponse, size_t* pSize,
                                   SOCKET hClientSocket)
{
    ASSERT(ppResponse != NULL);

    // This is what a bookmark file looks like:
    //
    //      <PACHANNEL 
    //      Name="Actor Tests"
    //      >
    //
    //
    //      <PAARTICLE
    //      Name		="AOL Europe Strikes Deal, HTML News Article"
    //      url		="file://c:/PublicAccess/Test/00007123.htm"
    //      >
    //      </PAARTICLE>
    //      </PACHANNEL>
    //

    // The categoryId to us is in the CategoryMapping array and will have a
    // 'category id' field (bits 31:16) matching this category id
   	UInt32 categoryId = 0;

    DataKey dkRequest = pRequest->GetCategoryId();
    UInt32 catId = EXTRACT_CATEGORY(dkRequest);

    for (UInt j = 0; categoryNames[j].lpChannelName != NULL; j++)
    {
        if (bookmarkURL != categoryNames[j].type)
            continue;
        UInt32 lstId = EXTRACT_CATEGORY(categoryNames[j].dkCategoryId);
        if (lstId == catId)
        {
            categoryId = categoryNames[j].dkCategoryId;
            break;
        }
    }

    SOCKADDR_IN sockAddr;
   	memset(&sockAddr,0,sizeof(sockAddr));
    int namelen = sizeof(sockAddr);
    int socketStatus = getsockname(hClientSocket, (struct sockaddr *)&sockAddr, &namelen); 
    ASSERT(0 == socketStatus);
    unsigned short int port = ntohs(sockAddr.sin_port);
    unsigned long int  addr = ntohl(sockAddr.sin_addr.s_addr);

    CString strChannel;
    CString strEntityBody;

    CString strChannelName = pCategory->lpChannelName;

    strChannel.Format("<%s\nName=\"%s\"\n>\n\n", kszCHANNEL, strChannelName);

    strEntityBody += strChannel;
    CString strURL;
    LPSTR lpAddress = inet_ntoa(sockAddr.sin_addr);
    strURL.Format("http://%s:%d/FIDO-1/%d-",lpAddress, port, categoryId);

    UInt lastId = pRequest->GetLastId();
    for (UInt i = 0; i < 5; i++)
    {
        CString strBM1;
        strBM1.Format("   <%s\n   Name=\"Headline #%d\"\n", kszBOOKMARK, i+1);
        strEntityBody+= strBM1;
        CString strBM2;

        // Our URL is as follows:
        //
        // http://<faultsim ip>:<faultsim port>/FIDO-1/<catId>-<lastId>/synth.html"
        //
        CString url;
        url.Format("   url=\"%s%d/synth.htm\"\n   >\n   </%s>\n", strURL, (i + lastId), kszBOOKMARK);
        strEntityBody += url;
    }

    CString strEnd;
    strEnd.Format("</%s>\n", kszCHANNEL);
    strEntityBody += strEnd;

    *pSize = strEntityBody.GetLength();
    if (0 != *pSize)
    {
        *ppResponse = new char[*pSize];
        if (*ppResponse != NULL)
        {
            memcpy(*ppResponse, (LPCSTR)strEntityBody, *pSize);
            return RESPONSE_GENERATED;
        }
    }

    return RESPONSE_ERROR;
}

CGenerateData::genStatus CGenerateData::GenerateBookMarkURL(const CHttpRequestParser* pRequest,
                                   const CategoryMapping* pCategory, 
                                   void** ppResponse, size_t* pSize)
{
    ASSERT(ppResponse != NULL);

    // Okay a synthesized bookmark file (see ::GenerateBookMark()) is being
    // used and the client has just requested a URL in the bookmark file, so
    // we have to create it.
    // We'll just use the ::GenerateArticle() routine...

    UInt32 articleIndex = pRequest->GetLastId();
    CString strHTML;
    genStatus status = GenerateHTML(pRequest, pCategory, strHTML, articleIndex);
    ASSERT(status == RESPONSE_GENERATED);
    if (RESPONSE_GENERATED != status)
    {
        ASSERT(FALSE);  
        return status;
    }

    *pSize = strHTML.GetLength();
    if (0 != *pSize)
    {
        *ppResponse = new char[*pSize];
        if (*ppResponse != NULL)
        {
            memcpy(*ppResponse, (LPCSTR)strHTML, *pSize);
            return RESPONSE_GENERATED;
        }
    }
    return RESPONSE_ERROR;
}