XMLRPC_VALUE XMLRPC_VectorNext(XMLRPC_VALUE value) {
   XMLRPC_VALUE xReturn = NULL;
   if(value && value->type == xmlrpc_vector && value->v) {
      xReturn = (XMLRPC_VALUE)Q_Next(value->v->q);
   }
   return xReturn;
}