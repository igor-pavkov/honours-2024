void XMLRPC_SetValueInt(XMLRPC_VALUE value, int val) {
   if(value) {
      value->type = xmlrpc_int;
      value->i = val;
   }
}