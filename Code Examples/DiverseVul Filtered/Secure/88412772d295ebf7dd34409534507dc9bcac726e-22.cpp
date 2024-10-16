static XMLRPC_VALUE map_expat_errors(XML_ELEM_ERROR error) {
   XMLRPC_VALUE xReturn = NULL;
   if(error) {
      XMLRPC_ERROR_CODE code;
      char buf[1024];
      snprintf(buf, sizeof(buf), 
               "error occurred at line %ld, column %ld, byte index %ld", 
					 error->line, error->column, error->byte_index);

      /* expat specific errors */
      switch(error->parser_code) {
      case XML_ERROR_UNKNOWN_ENCODING:
         code = xmlrpc_error_parse_unknown_encoding;
         break;
      case XML_ERROR_INCORRECT_ENCODING:
         code = xmlrpc_error_parse_bad_encoding;
         break;
      default:
         code = xmlrpc_error_parse_xml_syntax;
         break;
      }
      xReturn = XMLRPC_UtilityCreateFault(code, buf);
   }
   return xReturn;
}