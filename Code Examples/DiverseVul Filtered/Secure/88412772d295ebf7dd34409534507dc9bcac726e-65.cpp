XMLRPC_VALUE XMLRPC_CreateValueEmpty() {
   XMLRPC_VALUE v = calloc(1, sizeof(STRUCT_XMLRPC_VALUE));
   if(v) {
#ifdef XMLRPC_DEBUG_REFCOUNT
		printf ("calloc'd 0x%x\n", v);
#endif
      v->type = xmlrpc_empty;
      simplestring_init(&v->id);
      simplestring_init(&v->str);
   }
   return v;
}