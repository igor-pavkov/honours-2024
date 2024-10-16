XMLRPC_REQUEST XMLRPC_REQUEST_FromXML (const char *in_buf, int len, 
													XMLRPC_REQUEST_INPUT_OPTIONS in_options) {
   XMLRPC_REQUEST request = XMLRPC_RequestNew();
   STRUCT_XML_ELEM_ERROR error = {0};

   if(request) {
		xml_element *root_elem =
		xml_elem_parse_buf (in_buf, len,
								  (in_options ? &in_options->xml_elem_opts : NULL),
								  &error);

      if(root_elem) {
         if(!strcmp(root_elem->name, "simpleRPC")) {
            request->output.version = xmlrpc_version_simple;
            xml_element_to_DANDARPC_REQUEST(request, root_elem);
         }
			else if (!strcmp (root_elem->name, "SOAP-ENV:Envelope")) {
				request->output.version = xmlrpc_version_soap_1_1;
				xml_element_to_SOAP_REQUEST (request, root_elem);
			}
         else {
            request->output.version = xmlrpc_version_1_0;
            xml_element_to_XMLRPC_REQUEST(request, root_elem);
         }
         xml_elem_free(root_elem);
      }
      else {
         if(error.parser_error) {
				XMLRPC_RequestSetError (request, map_expat_errors (&error));
         }
      }
   }

   return request;
}