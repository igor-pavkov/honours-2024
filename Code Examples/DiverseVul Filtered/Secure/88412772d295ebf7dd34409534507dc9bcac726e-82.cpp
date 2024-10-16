XMLRPC_VALUE XMLRPC_CreateVector(const char* id, XMLRPC_VECTOR_TYPE type) {
   XMLRPC_VALUE val = NULL;

   val = XMLRPC_CreateValueEmpty();
   if(val) {
      if(XMLRPC_SetIsVector(val, type)) {
         if(id) {
            const char *pSVI = NULL;

            pSVI = XMLRPC_SetValueID(val, id, 0);
            if(NULL == pSVI) {
               val = NULL;
            }
         }
      }
      else {
         val = NULL;
      }
   }
   return val;
}