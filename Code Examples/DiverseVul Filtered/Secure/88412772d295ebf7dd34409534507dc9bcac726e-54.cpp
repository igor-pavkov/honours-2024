const char* XMLRPC_RequestGetMethodName(XMLRPC_REQUEST request) {
   return request ? request->methodName.str : NULL;
}