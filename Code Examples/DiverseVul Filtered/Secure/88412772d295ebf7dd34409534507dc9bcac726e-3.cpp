XMLRPC_VALUE_TYPE XMLRPC_GetValueType(XMLRPC_VALUE value) {
   return value ? value->type : xmlrpc_empty;
}