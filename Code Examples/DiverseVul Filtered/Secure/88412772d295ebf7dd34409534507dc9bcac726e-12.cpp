XMLRPC_VALUE XMLRPC_CopyValue(XMLRPC_VALUE value) {
   if(value) {
      value->iRefCount ++;
#ifdef XMLRPC_DEBUG_REFCOUNT
      if(value->id.str) {
			printf ("incremented refcount of %s, now %i\n", value->id.str,
					  value->iRefCount);
		}
		else {
			printf ("incremented refcount of 0x%x, now %i\n", value,
					  value->iRefCount);
      }
#endif
   }
   return value;
}