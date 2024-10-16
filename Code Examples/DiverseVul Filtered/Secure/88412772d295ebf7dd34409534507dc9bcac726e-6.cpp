int XMLRPC_GetValueInt(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_int) ? value->i : 0);
}