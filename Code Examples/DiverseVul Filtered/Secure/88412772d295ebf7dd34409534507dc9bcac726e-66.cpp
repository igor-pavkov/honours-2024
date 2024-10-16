double XMLRPC_GetValueDouble(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_double) ? value->d : 0);
}