XMLRPC_VALUE XMLRPC_CreateValueDateTime(const char* id, time_t time) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueDateTime(val, time);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}