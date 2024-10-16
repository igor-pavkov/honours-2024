XMLRPC_VALUE XMLRPC_CreateValueBoolean(const char* id, int i) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueBoolean(val, i);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}