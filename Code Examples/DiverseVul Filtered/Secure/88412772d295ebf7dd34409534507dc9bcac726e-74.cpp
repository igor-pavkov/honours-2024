static XMLRPC_OPTIONS XMLRPC_GetDefaultOptions() {
   static STRUCT_XMLRPC_OPTIONS options = {
      xmlrpc_case_exact,
      xmlrpc_case_sensitive
   };
   return &options;
}