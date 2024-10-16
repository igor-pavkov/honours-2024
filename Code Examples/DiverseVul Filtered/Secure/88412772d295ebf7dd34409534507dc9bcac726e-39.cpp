int XMLRPC_VectorSize(XMLRPC_VALUE value) {
   int size = 0;
   if(value && value->type == xmlrpc_vector && value->v) {
      size = Q_Size(value->v->q);
   }
   return size;
}