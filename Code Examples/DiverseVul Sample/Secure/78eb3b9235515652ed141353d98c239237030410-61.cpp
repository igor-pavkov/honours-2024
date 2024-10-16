static int req_parseargs(lua_State *L)
{
    apr_table_t *form_table;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    lua_newtable(L);
    lua_newtable(L);            /* [table, table] */
    ap_args_to_table(r, &form_table);
    apr_table_do(req_aprtable2luatable_cb, L, form_table, NULL);
    return 2;                   /* [table<string, string>, table<string, array<string>>] */
}