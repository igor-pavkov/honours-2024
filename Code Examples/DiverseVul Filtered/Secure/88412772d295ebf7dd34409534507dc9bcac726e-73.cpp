XMLRPC_VALUE XMLRPC_ServerCallMethod(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData) {
   XMLRPC_VALUE xReturn = NULL;

   /* check for error set during request parsing / generation */
   if(request && request->error) {
      xReturn = XMLRPC_CopyValue(request->error);
   }
	else if (server && request) {
		XMLRPC_Callback cb =
		XMLRPC_ServerFindMethod (server, request->methodName.str);
      if(cb) {
         xReturn = cb(server, request, userData);
      }
      else {
			xReturn =
			XMLRPC_UtilityCreateFault (xmlrpc_error_unknown_method,
												request->methodName.str);
      }
   }
   return xReturn;
}