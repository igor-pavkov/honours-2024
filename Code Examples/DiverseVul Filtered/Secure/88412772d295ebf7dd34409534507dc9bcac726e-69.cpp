XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestSetOutputOptions(XMLRPC_REQUEST request, XMLRPC_REQUEST_OUTPUT_OPTIONS output) {
   if(request && output) {
		memcpy (&request->output, output,
				  sizeof (STRUCT_XMLRPC_REQUEST_OUTPUT_OPTIONS));
      return &request->output;
   }
   return NULL;
}