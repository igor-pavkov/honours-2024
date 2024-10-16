const char* XMLRPC_GetResponseFaultString (XMLRPC_REQUEST response) {
   return XMLRPC_GetValueFaultString( XMLRPC_RequestGetData(response) );
}