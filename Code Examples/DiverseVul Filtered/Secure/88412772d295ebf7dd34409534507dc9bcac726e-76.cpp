const char* XMLRPC_GetValueID(XMLRPC_VALUE value) {
    return (const char*)((value && value->id.len) ? value->id.str : 0);
}