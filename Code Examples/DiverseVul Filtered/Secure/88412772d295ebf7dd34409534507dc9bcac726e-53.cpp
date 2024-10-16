XMLRPC_REQUEST_TYPE XMLRPC_RequestGetRequestType(XMLRPC_REQUEST request) {
   return request ? request->request_type : xmlrpc_request_none;
}