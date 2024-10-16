void XMLRPC_SetValueDouble(XMLRPC_VALUE value, double val) {
   if(value) {
      value->type = xmlrpc_double;
      value->d = val;
   }
}