int XMLRPC_GetResponseFaultCode(XMLRPC_REQUEST response) {
   return XMLRPC_GetValueFaultCode( XMLRPC_RequestGetData(response) );
}