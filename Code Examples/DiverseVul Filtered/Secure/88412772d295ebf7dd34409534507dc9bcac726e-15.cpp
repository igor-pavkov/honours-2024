int XMLRPC_GetValueStringLen(XMLRPC_VALUE value) {
    return ((value) ? value->str.len : 0);
}