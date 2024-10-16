static void mod_wstunnel_merge_config_cpv(plugin_config * const pconf, const config_plugin_value_t * const cpv) {
    switch (cpv->k_id) { /* index into static config_plugin_keys_t cpk[] */
      case 0: /* wstunnel.server */
        if (cpv->vtype == T_CONFIG_LOCAL) {
            gw_plugin_config * const gw = cpv->v.v;
            pconf->gw.exts      = gw->exts;
            pconf->gw.exts_auth = gw->exts_auth;
            pconf->gw.exts_resp = gw->exts_resp;
        }
        break;
      case 1: /* wstunnel.balance */
        /*if (cpv->vtype == T_CONFIG_LOCAL)*//*always true here for this param*/
            pconf->gw.balance = (int)cpv->v.u;
        break;
      case 2: /* wstunnel.debug */
        pconf->gw.debug = (int)cpv->v.u;
        break;
      case 3: /* wstunnel.map-extensions */
        pconf->gw.ext_mapping = cpv->v.a;
        break;
      case 4: /* wstunnel.frame-type */
        pconf->frame_type = cpv->v.u;
        break;
      case 5: /* wstunnel.origins */
        pconf->origins = cpv->v.a;
        break;
      case 6: /* wstunnel.ping-interval */
        pconf->ping_interval = cpv->v.shrt;
        break;
      default:/* should not happen */
        return;
    }
}