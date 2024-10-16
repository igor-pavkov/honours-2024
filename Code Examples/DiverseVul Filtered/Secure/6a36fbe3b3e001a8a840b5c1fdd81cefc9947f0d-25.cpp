int xmlSAXUserParseMemory(xmlSAXHandlerPtr sax, void *user_data,
			  const char *buffer, int size) {
    int ret = 0;
    xmlParserCtxtPtr ctxt;

    xmlInitParser();

    ctxt = xmlCreateMemoryParserCtxt(buffer, size);
    if (ctxt == NULL) return -1;
    if (ctxt->sax != (xmlSAXHandlerPtr) &xmlDefaultSAXHandler)
        xmlFree(ctxt->sax);
    ctxt->sax = sax;
    xmlDetectSAX2(ctxt);

    if (user_data != NULL)
	ctxt->userData = user_data;

    xmlParseDocument(ctxt);

    if (ctxt->wellFormed)
	ret = 0;
    else {
        if (ctxt->errNo != 0)
	    ret = ctxt->errNo;
	else
	    ret = -1;
    }
    if (sax != NULL)
        ctxt->sax = NULL;
    if (ctxt->myDoc != NULL) {
        xmlFreeDoc(ctxt->myDoc);
	ctxt->myDoc = NULL;
    }
    xmlFreeParserCtxt(ctxt);

    return ret;
}