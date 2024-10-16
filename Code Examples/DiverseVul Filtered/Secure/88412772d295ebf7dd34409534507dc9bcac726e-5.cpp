const char* XMLRPC_GetValueFaultString (XMLRPC_VALUE value) {
   return XMLRPC_VectorGetStringWithID(value, "faultString");
}