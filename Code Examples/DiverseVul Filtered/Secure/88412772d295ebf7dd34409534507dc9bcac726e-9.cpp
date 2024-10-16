XMLRPC_VALUE XMLRPC_VectorRewind(XMLRPC_VALUE value) {
   XMLRPC_VALUE xReturn = NULL;
   if(value && value->type == xmlrpc_vector && value->v) {
      xReturn = (XMLRPC_VALUE)Q_Head(value->v->q);
   }
   return xReturn;
}