XMLRPC_VALUE XMLRPC_VALUE_FromXML (const char *in_buf, int len, XMLRPC_REQUEST_INPUT_OPTIONS in_options) {
   XMLRPC_VALUE xResponse = NULL;
   XMLRPC_REQUEST req = XMLRPC_REQUEST_FromXML(in_buf, len, in_options);

   if(req) {
      xResponse = req->io;
      XMLRPC_RequestFree(req, 0);
   }
   return xResponse;
}