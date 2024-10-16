static int xmlSaveSwitchEncoding(xmlSaveCtxtPtr ctxt, const char *encoding) {
    xmlOutputBufferPtr buf = ctxt->buf;

    if ((encoding != NULL) && (buf->encoder == NULL) && (buf->conv == NULL)) {
	buf->encoder = xmlFindCharEncodingHandler((const char *)encoding);
	if (buf->encoder == NULL) {
	    xmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, NULL,
		       (const char *)encoding);
	    return(-1);
	}
	buf->conv = xmlBufCreate();
	if (buf->conv == NULL) {
	    xmlCharEncCloseFunc(buf->encoder);
	    xmlSaveErrMemory("creating encoding buffer");
	    return(-1);
	}
	/*
	 * initialize the state, e.g. if outputting a BOM
	 */
        xmlCharEncOutput(buf, 1);
    }
    return(0);
}