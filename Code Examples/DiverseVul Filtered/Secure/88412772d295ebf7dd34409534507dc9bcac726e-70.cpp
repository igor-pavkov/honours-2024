XMLRPC_VECTOR_TYPE XMLRPC_GetVectorType(XMLRPC_VALUE value) {
   return(value && value->v) ? value->v->type : xmlrpc_none;
}