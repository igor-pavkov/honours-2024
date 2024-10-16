void XMLRPC_SetValueBase64(XMLRPC_VALUE value, const char* s, int len) {
   if(value && s) {
      simplestring_clear(&value->str);
      (len > 0) ? simplestring_addn(&value->str, s, len) :
                  simplestring_add(&value->str, s);
      value->type = xmlrpc_base64;
   }
}