XMLRPC_VALUE XMLRPC_CreateValueString(const char* id, const char* val, int len) {
   XMLRPC_VALUE value = NULL;
   if(val) {
      value = XMLRPC_CreateValueEmpty();
      if(value) {
         XMLRPC_SetValueString(value, val, len);
         if(id) {
            XMLRPC_SetValueID(value, id, 0);
         }
      }
   }
   return value;
}