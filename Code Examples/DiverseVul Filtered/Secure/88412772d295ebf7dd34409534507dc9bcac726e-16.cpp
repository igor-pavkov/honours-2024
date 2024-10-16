char* XMLRPC_REQUEST_ToXML(XMLRPC_REQUEST request, int* buf_len) {
      char* pRet = NULL;
	if (request) {
		xml_element *root_elem = NULL;
		if (request->output.version == xmlrpc_version_simple) {
			root_elem = DANDARPC_REQUEST_to_xml_element (request);
		}
		else if (request->output.version == xmlrpc_version_1_0 ||
					request->output.version == xmlrpc_version_none) {
			root_elem = XMLRPC_REQUEST_to_xml_element (request);
		}
		else if (request->output.version == xmlrpc_version_soap_1_1) {
			root_elem = SOAP_REQUEST_to_xml_element (request);
		}

      if(root_elem) {
			pRet =
			xml_elem_serialize_to_string (root_elem,
													&request->output.xml_elem_opts,
													buf_len);
         xml_elem_free(root_elem);
      }
   }
	return pRet;
}