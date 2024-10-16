XMLRPC_VALUE XMLRPC_UtilityCreateFault(int fault_code, const char* fault_string) {
   XMLRPC_VALUE xOutput = NULL;

   char* string = NULL;
   simplestring description;
   simplestring_init(&description);

   switch (fault_code) {
	case xmlrpc_error_parse_xml_syntax:
		string = xmlrpc_error_parse_xml_syntax_str;
		break;
	case xmlrpc_error_parse_unknown_encoding:
		string = xmlrpc_error_parse_unknown_encoding_str;
		break;
	case xmlrpc_error_parse_bad_encoding:
		string = xmlrpc_error_parse_bad_encoding_str;
		break;
	case xmlrpc_error_invalid_xmlrpc:
		string = xmlrpc_error_invalid_xmlrpc_str;
		break;
	case xmlrpc_error_unknown_method:
		string = xmlrpc_error_unknown_method_str;
		break;
	case xmlrpc_error_invalid_params:
		string = xmlrpc_error_invalid_params_str;
		break;
	case xmlrpc_error_internal_server:
		string = xmlrpc_error_internal_server_str;
		break;
	case xmlrpc_error_application:
		string = xmlrpc_error_application_str;
		break;
	case xmlrpc_error_system:
		string = xmlrpc_error_system_str;
		break;
	case xmlrpc_error_transport:
		string = xmlrpc_error_transport_str;
		break;
   }

   simplestring_add(&description, string);

   if(string && fault_string) {
      simplestring_add(&description, "\n\n");
   }
   simplestring_add(&description, fault_string);


   if(description.len) {
      xOutput = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);

		XMLRPC_VectorAppendString (xOutput, "faultString", description.str,
											description.len);
      XMLRPC_VectorAppendInt(xOutput, "faultCode", fault_code);
   }

   simplestring_free(&description);

   return xOutput;
}