const char* XMLRPC_RequestSetMethodName(XMLRPC_REQUEST request, const char* methodName) {
   if(request) {
      simplestring_clear(&request->methodName);
      simplestring_add(&request->methodName, methodName);
      return request->methodName.str;
   }
   return NULL;
}