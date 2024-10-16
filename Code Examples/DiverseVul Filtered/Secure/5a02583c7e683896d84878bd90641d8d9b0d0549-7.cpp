static void xmlGROW (xmlParserCtxtPtr ctxt) {
    ptrdiff_t curEnd = ctxt->input->end - ctxt->input->cur;
    ptrdiff_t curBase = ctxt->input->cur - ctxt->input->base;

    if (((curEnd > XML_MAX_LOOKUP_LIMIT) ||
         (curBase > XML_MAX_LOOKUP_LIMIT)) &&
         ((ctxt->input->buf) &&
          (ctxt->input->buf->readcallback != xmlInputReadCallbackNop)) &&
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
    if ((ctxt->input->cur != NULL) && (*ctxt->input->cur == 0))
        xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
}