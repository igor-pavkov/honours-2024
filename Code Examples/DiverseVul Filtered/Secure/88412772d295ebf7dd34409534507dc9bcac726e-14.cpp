XMLRPC_VALUE XMLRPC_RequestSetData(XMLRPC_REQUEST request, XMLRPC_VALUE data) {
   if(request && data) {
		if (request->io) {
			XMLRPC_CleanupValue (request->io);
		}
      request->io = XMLRPC_CopyValue(data);
      return request->io;
   }
   return NULL;
}