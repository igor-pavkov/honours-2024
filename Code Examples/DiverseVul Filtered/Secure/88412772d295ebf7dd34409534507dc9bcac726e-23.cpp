const char* XMLRPC_GetValueDateTime_ISO8601(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_datetime) ? value->str.str : 0);
}