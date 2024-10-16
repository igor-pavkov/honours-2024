static int lua_ap_get_active_config(lua_State *L) {
    ap_directive_t *subdir;
    ap_directive_t *dir = ap_conftree;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    
    for (dir = ap_conftree; dir; dir = dir->next) {
        if (ap_strcasestr(dir->directive, "<virtualhost") && dir->first_child) {
            for (subdir = dir->first_child; subdir; subdir = subdir->next) {
                if (ap_strcasecmp_match(subdir->directive, "servername") &&
                        !ap_strcasecmp_match(r->hostname, subdir->args)) {
                    read_cfg_tree(L, r, dir->first_child);
                    return 1;
                }
                if (ap_strcasecmp_match(subdir->directive, "serveralias") &&
                        !ap_strcasecmp_match(r->hostname, subdir->args)) {
                    read_cfg_tree(L, r, dir->first_child);
                    return 1;
                }
            }
        }
    }     
    return 0;
}