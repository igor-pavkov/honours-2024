static int xmlSaveClearEncoding(xmlSaveCtxtPtr ctxt) {
    xmlOutputBufferPtr buf = ctxt->buf;
    xmlOutputBufferFlush(buf);
    xmlCharEncCloseFunc(buf->encoder);
    xmlBufFree(buf->conv);
    buf->encoder = NULL;
    buf->conv = NULL;
    return(0);
}