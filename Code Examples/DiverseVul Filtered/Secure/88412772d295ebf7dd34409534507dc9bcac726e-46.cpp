const char* XMLRPC_GetValueBase64(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_base64) ? value->str.str : 0);
}