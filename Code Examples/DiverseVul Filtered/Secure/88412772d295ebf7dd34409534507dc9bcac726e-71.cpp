void XMLRPC_SetValueBoolean(XMLRPC_VALUE value, int val) {
   if(value) {
      value->type = xmlrpc_boolean;
      value->i = val ? 1 : 0;
   }
}