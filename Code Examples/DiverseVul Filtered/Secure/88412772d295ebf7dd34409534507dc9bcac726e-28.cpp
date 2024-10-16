const char *XMLRPC_SetValueString(XMLRPC_VALUE value, const char* val, int len) {
   char *pRetval = NULL;
   if(value && val) {
      simplestring_clear(&value->str);
      (len > 0) ? simplestring_addn(&value->str, val, len) :
                  simplestring_add(&value->str, val);
      value->type = xmlrpc_string;
      pRetval = (char *)value->str.str;
   }

   return pRetval;
}