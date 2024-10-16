void StoreOriginalTextInToken(TidyDocImpl* doc, Node* node, uint count)
{
    if (!doc->storeText)
        return;

    if (count >= doc->docIn->otextlen)
        return;

    if (!doc->docIn->otextsize)
        return;

    if (count == 0)
    {
        node->otext = doc->docIn->otextbuf;
        doc->docIn->otextbuf = NULL;
        doc->docIn->otextlen = 0;
        doc->docIn->otextsize = 0;
    }
    else
    {
        uint len = doc->docIn->otextlen;
        tmbstr buf1 = (tmbstr)TidyDocAlloc(doc, len - count + 1);
        tmbstr buf2 = (tmbstr)TidyDocAlloc(doc, count + 1);
        uint i, j;

        /* strncpy? */

        for (i = 0; i < len - count; ++i)
            buf1[i] = doc->docIn->otextbuf[i];

        buf1[i] = 0;

        for (j = 0; j + i < len; ++j)
            buf2[j] = doc->docIn->otextbuf[j + i];

        buf2[j] = 0;

        TidyDocFree(doc, doc->docIn->otextbuf);
        node->otext = buf1;
        doc->docIn->otextbuf = buf2;
        doc->docIn->otextlen = count;
        doc->docIn->otextsize = count + 1;
    }
}