char *_q_urlencode(const void *bin, size_t size)
{
    const char URLCHARTBL[256] = {
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* 00-0F */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* 10-1F */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-','.','/', /* 20-2F */
        '0','1','2','3','4','5','6','7','8','9',':', 0 , 0 , 0 , 0 , 0 , /* 30-3F */
        '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O', /* 40-4F */
        'P','Q','R','S','T','U','V','W','X','Y','Z', 0 ,'\\',0 , 0 ,'_', /* 50-5F */
        0 ,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o', /* 60-6f */
        'p','q','r','s','t','u','v','w','x','y','z', 0 , 0 , 0 , 0 , 0 , /* 70-7F */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* 80-8F */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* 90-9F */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* A0-AF */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* B0-BF */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* C0-CF */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* D0-DF */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* E0-EF */
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0   /* F0-FF */
    }; // 0 means must be encoded.

    if (bin == NULL) return NULL;
    if (size == 0) return strdup("");

    // malloc buffer
    char *pszEncStr = (char *)malloc((size * 3) + 1);
    if (pszEncStr == NULL) return NULL;

    char *pszEncPt = pszEncStr;
    char *pBinPt = (char *)bin;
    const char *pBinEnd = (bin + size - 1);
    for (; pBinPt <= pBinEnd; pBinPt++) {
        if (URLCHARTBL[(int)(*pBinPt)] != 0) {
            *pszEncPt++ = *pBinPt;
        } else {
            unsigned char cUpper4 = (*pBinPt >> 4);
            unsigned char cLower4 = (*pBinPt & 0x0F);

            *pszEncPt++ = '%';
            *pszEncPt++ = (cUpper4 < 0x0A) ? (cUpper4 + '0') : ((cUpper4 - 0x0A) + 'a');
            *pszEncPt++ = (cLower4 < 0x0A) ? (cLower4 + '0') : ((cLower4 - 0x0A) + 'a');
        }
    }
    *pszEncPt = '\0';

    return pszEncStr;
}