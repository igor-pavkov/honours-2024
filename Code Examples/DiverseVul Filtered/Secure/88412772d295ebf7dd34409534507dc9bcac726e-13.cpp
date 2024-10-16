XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestGetOutputOptions(XMLRPC_REQUEST request) {
   return request ? &request->output : NULL;
}