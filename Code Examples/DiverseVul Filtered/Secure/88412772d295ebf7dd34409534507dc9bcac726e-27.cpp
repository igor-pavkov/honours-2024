XMLRPC_REQUEST_TYPE XMLRPC_RequestSetRequestType (XMLRPC_REQUEST request,
																  XMLRPC_REQUEST_TYPE type) {
   if(request) {
      request->request_type = type;
      return request->request_type;
   }
   return xmlrpc_request_none;
}