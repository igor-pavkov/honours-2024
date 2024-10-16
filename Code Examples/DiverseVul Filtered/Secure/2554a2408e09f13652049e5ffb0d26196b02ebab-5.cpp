xmlFreeNodeList(xmlNodePtr cur) {
    xmlNodePtr next;
    xmlNodePtr parent;
    xmlDictPtr dict = NULL;
    size_t depth = 0;

    if (cur == NULL) return;
    if (cur->type == XML_NAMESPACE_DECL) {
	xmlFreeNsList((xmlNsPtr) cur);
	return;
    }
    if (cur->doc != NULL) dict = cur->doc->dict;
    while (1) {
        while ((cur->children != NULL) &&
               (cur->type != XML_DOCUMENT_NODE) &&
#ifdef LIBXML_DOCB_ENABLED
               (cur->type != XML_DOCB_DOCUMENT_NODE) &&
#endif
               (cur->type != XML_HTML_DOCUMENT_NODE) &&
               (cur->type != XML_DTD_NODE) &&
               (cur->type != XML_ENTITY_REF_NODE)) {
            cur = cur->children;
            depth += 1;
        }

        next = cur->next;
        parent = cur->parent;
	if ((cur->type == XML_DOCUMENT_NODE) ||
#ifdef LIBXML_DOCB_ENABLED
            (cur->type == XML_DOCB_DOCUMENT_NODE) ||
#endif
            (cur->type == XML_HTML_DOCUMENT_NODE)) {
            xmlFreeDoc((xmlDocPtr) cur);
        } else if (cur->type != XML_DTD_NODE) {

	    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
		xmlDeregisterNodeDefaultValue(cur);

	    if (((cur->type == XML_ELEMENT_NODE) ||
		 (cur->type == XML_XINCLUDE_START) ||
		 (cur->type == XML_XINCLUDE_END)) &&
		(cur->properties != NULL))
		xmlFreePropList(cur->properties);
	    if ((cur->type != XML_ELEMENT_NODE) &&
		(cur->type != XML_XINCLUDE_START) &&
		(cur->type != XML_XINCLUDE_END) &&
		(cur->type != XML_ENTITY_REF_NODE) &&
		(cur->content != (xmlChar *) &(cur->properties))) {
		DICT_FREE(cur->content)
	    }
	    if (((cur->type == XML_ELEMENT_NODE) ||
	         (cur->type == XML_XINCLUDE_START) ||
		 (cur->type == XML_XINCLUDE_END)) &&
		(cur->nsDef != NULL))
		xmlFreeNsList(cur->nsDef);

	    /*
	     * When a node is a text node or a comment, it uses a global static
	     * variable for the name of the node.
	     * Otherwise the node name might come from the document's
	     * dictionary
	     */
	    if ((cur->name != NULL) &&
		(cur->type != XML_TEXT_NODE) &&
		(cur->type != XML_COMMENT_NODE))
		DICT_FREE(cur->name)
	    xmlFree(cur);
	}

        if (next != NULL) {
	    cur = next;
        } else {
            if ((depth == 0) || (parent == NULL))
                break;
            depth -= 1;
            cur = parent;
            cur->children = NULL;
        }
    }
}