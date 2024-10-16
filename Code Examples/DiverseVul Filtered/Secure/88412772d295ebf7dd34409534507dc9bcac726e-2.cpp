int XMLRPC_AddValuesToVector(XMLRPC_VALUE target, ...) {
   int iRetval = 0;

   if(target) {
      if(target->type == xmlrpc_vector) {
         XMLRPC_VALUE v = NULL;
         va_list vl;

         va_start(vl, target);

         do {
            v = va_arg(vl, XMLRPC_VALUE);
            if(v) {
               if(!XMLRPC_AddValueToVector(target, v)) {
                  iRetval = 0;
                  break;
               }
            }
			}
			while (v);

         va_end(vl);

         if(NULL == v) {
            iRetval = 1;
         }
      }
   }
   return iRetval;
}