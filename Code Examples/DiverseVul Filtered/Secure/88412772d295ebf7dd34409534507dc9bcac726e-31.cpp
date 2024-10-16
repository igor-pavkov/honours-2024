XMLRPC_VALUE XMLRPC_RequestGetData(XMLRPC_REQUEST request) {
   return request ? request->io : NULL;
}