static void mod_wstunnel_patch_config(request_st * const r, plugin_data * const p) {
    memcpy(&p->conf, &p->defaults, sizeof(plugin_config));
    for (int i = 1, used = p->nconfig; i < used; ++i) {
        if (config_check_cond(r, (uint32_t)p->cvlist[i].k_id))
            mod_wstunnel_merge_config(&p->conf, p->cvlist+p->cvlist[i].v.u2[0]);
    }
}