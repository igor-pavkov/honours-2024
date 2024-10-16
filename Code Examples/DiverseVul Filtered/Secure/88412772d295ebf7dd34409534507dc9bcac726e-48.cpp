XMLRPC_VALUE XMLRPC_CreateValueDateTime_ISO8601(const char* id, const char *s) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueDateTime_ISO8601(val, s);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}