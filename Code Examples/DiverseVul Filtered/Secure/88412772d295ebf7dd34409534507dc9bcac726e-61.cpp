const char* type_to_str(XMLRPC_VALUE_TYPE type, XMLRPC_VECTOR_TYPE vtype) {
    switch(type) {
       case xmlrpc_none:
          return "none";
       case xmlrpc_empty:
          return "empty";
       case xmlrpc_base64:
          return "base64";
       case xmlrpc_boolean:
          return "boolean";
       case xmlrpc_datetime:
          return "datetime";
       case xmlrpc_double:
          return "double";
       case xmlrpc_int:
          return "int";
       case xmlrpc_string:
          return "string";
       case xmlrpc_vector:
          switch(vtype) {
             case xmlrpc_vector_none:
                return "none";
             case xmlrpc_vector_array:
                return "array";
             case xmlrpc_vector_mixed:
                return "mixed vector (struct)";
             case xmlrpc_vector_struct:
                return "struct";
          }
    }
    return "unknown";
}