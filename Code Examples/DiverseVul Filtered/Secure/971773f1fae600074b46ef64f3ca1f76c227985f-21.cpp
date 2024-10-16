static void mod_wstunnel_merge_config(plugin_config * const pconf, const config_plugin_value_t *cpv) {
    do {
        mod_wstunnel_merge_config_cpv(pconf, cpv);
    } while ((++cpv)->k_id != -1);
}