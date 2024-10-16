void XMLRPC_RequestFree(XMLRPC_REQUEST request, int bFreeIO) {
   if(request) {
      simplestring_free(&request->methodName);

      if(request->io && bFreeIO) {
         XMLRPC_CleanupValue(request->io);
      }
      if(request->error) {
         XMLRPC_CleanupValue(request->error);
      }
      my_free(request);
   }
}