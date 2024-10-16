int XMLRPC_ResponseIsFault(XMLRPC_REQUEST response) {
   return XMLRPC_ValueIsFault( XMLRPC_RequestGetData(response) );
}