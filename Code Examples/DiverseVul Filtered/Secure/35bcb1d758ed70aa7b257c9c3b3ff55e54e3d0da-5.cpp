static void xmlGROW (xmlParserCtxtPtr ctxt) {
    unsigned long curEnd = ctxt->input->end - ctxt->input->cur;
    unsigned long curBase = ctxt->input->cur - ctxt->input->base;

    if (((curEnd > (unsigned long) XML_MAX_LOOKUP_LIMIT) ||
         (curBase > (unsigned long) XML_MAX_LOOKUP_LIMIT)) &&
         ((ctxt->input->buf) && (ctxt->input->buf->readcallback != (xmlInputReadCallback) xmlNop)) &&
        ((ctxt->options & XML_PARSE_HUGE) == 0)) {
        xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR, "Huge input lookup");
        xmlHaltParser(ctxt);
	return;
    }
    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
    if ((ctxt->input->cur > ctxt->input->end) ||
        (ctxt->input->cur < ctxt->input->base)) {
        xmlHaltParser(ctxt);
        xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR, "cur index out of bound");
	return;
    }
    if ((ctxt->input->cur != NULL) && (*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0))
	    xmlPopInput(ctxt);
}