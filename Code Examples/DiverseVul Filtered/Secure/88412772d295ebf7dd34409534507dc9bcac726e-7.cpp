int XMLRPC_GetValueBoolean(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_boolean) ? value->i : 0);
}