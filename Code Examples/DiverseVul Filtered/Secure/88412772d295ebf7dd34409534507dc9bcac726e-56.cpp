int XMLRPC_ValueIsFault (XMLRPC_VALUE value) {
   if( XMLRPC_VectorGetValueWithID(value, "faultCode") &&
       XMLRPC_VectorGetValueWithID(value, "faultString") ) {
      return 1;
   }
   return 0;
}