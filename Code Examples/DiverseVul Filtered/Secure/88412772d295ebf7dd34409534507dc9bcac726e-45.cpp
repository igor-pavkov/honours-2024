const char* XMLRPC_GetValueString(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_string) ? value->str.str : 0);
}