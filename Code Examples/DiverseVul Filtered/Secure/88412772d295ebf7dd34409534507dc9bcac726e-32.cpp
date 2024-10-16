XMLRPC_VALUE XMLRPC_CreateValueDouble(const char* id, double d) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueDouble(val, d);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}