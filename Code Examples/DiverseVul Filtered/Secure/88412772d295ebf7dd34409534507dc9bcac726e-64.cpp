int XMLRPC_GetValueFaultCode (XMLRPC_VALUE value) {
   return XMLRPC_VectorGetIntWithID(value, "faultCode");
}