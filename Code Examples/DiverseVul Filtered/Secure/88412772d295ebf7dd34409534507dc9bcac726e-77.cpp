XMLRPC_CASE_COMPARISON XMLRPC_SetDefaultIdCaseComparison(XMLRPC_CASE_COMPARISON id_case_compare) {
   XMLRPC_OPTIONS options = XMLRPC_GetDefaultOptions();
   options->id_case_compare = id_case_compare;
   return options->id_case_compare;
}