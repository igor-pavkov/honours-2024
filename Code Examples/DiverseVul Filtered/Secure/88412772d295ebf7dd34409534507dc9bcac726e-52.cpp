time_t XMLRPC_GetValueDateTime(XMLRPC_VALUE value) {
    return (time_t)((value && value->type == xmlrpc_datetime) ? value->i : 0);
}