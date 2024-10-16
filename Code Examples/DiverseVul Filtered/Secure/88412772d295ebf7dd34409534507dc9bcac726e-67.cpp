XMLRPC_REQUEST XMLRPC_RequestNew() {
   XMLRPC_REQUEST xRequest = calloc(1, sizeof(STRUCT_XMLRPC_REQUEST));
   if(xRequest) {
      simplestring_init(&xRequest->methodName);
   }
   return xRequest;
}