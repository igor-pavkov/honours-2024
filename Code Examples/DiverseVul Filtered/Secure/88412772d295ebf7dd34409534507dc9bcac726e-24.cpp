XMLRPC_VALUE XMLRPC_CreateValueBase64(const char* id, const char* s, int len) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueBase64(val, s, len);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}