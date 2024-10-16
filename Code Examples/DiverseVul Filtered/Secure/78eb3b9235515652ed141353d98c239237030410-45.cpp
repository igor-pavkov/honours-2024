static int lua_ap_get_config(lua_State *L) {
    request_rec *r = ap_lua_check_request_rec(L, 1);   
    read_cfg_tree(L, r, ap_conftree);
    
    return 1;
}