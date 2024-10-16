SETDEFAULTS_FUNC(mod_wstunnel_set_defaults) {
    static const config_plugin_keys_t cpk[] = {
      { CONST_STR_LEN("wstunnel.server"),
        T_CONFIG_ARRAY_KVARRAY,
        T_CONFIG_SCOPE_CONNECTION }
     ,{ CONST_STR_LEN("wstunnel.balance"),
        T_CONFIG_STRING,
        T_CONFIG_SCOPE_CONNECTION }
     ,{ CONST_STR_LEN("wstunnel.debug"),
        T_CONFIG_INT,
        T_CONFIG_SCOPE_CONNECTION }
     ,{ CONST_STR_LEN("wstunnel.map-extensions"),
        T_CONFIG_ARRAY_KVSTRING,
        T_CONFIG_SCOPE_CONNECTION }
     ,{ CONST_STR_LEN("wstunnel.frame-type"),
        T_CONFIG_STRING,
        T_CONFIG_SCOPE_CONNECTION }
     ,{ CONST_STR_LEN("wstunnel.origins"),
        T_CONFIG_ARRAY_VLIST,
        T_CONFIG_SCOPE_CONNECTION }
     ,{ CONST_STR_LEN("wstunnel.ping-interval"),
        T_CONFIG_SHORT,
        T_CONFIG_SCOPE_CONNECTION }
     ,{ NULL, 0,
        T_CONFIG_UNSET,
        T_CONFIG_SCOPE_UNSET }
    };

    plugin_data * const p = p_d;
    if (!config_plugin_values_init(srv, p, cpk, "mod_wstunnel"))
        return HANDLER_ERROR;

    /* process and validate config directives
     * (init i to 0 if global context; to 1 to skip empty global context) */
    for (int i = !p->cvlist[0].v.u2[1]; i < p->nconfig; ++i) {
        config_plugin_value_t *cpv = p->cvlist + p->cvlist[i].v.u2[0];
        gw_plugin_config *gw = NULL;
        for (; -1 != cpv->k_id; ++cpv) {
            switch (cpv->k_id) {
              case 0: /* wstunnel.server */
                gw = calloc(1, sizeof(gw_plugin_config));
                force_assert(gw);
                if (!gw_set_defaults_backend(srv, (gw_plugin_data *)p, cpv->v.a,
                                             gw, 0, cpk[cpv->k_id].k)) {
                    gw_plugin_config_free(gw);
                    return HANDLER_ERROR;
                }
                /* error if "mode" = "authorizer";
                 * wstunnel can not act as authorizer */
                /*(check after gw_set_defaults_backend())*/
                if (gw->exts_auth && gw->exts_auth->used) {
                    log_error(srv->errh, __FILE__, __LINE__,
                      "%s must not define any hosts with "
                      "attribute \"mode\" = \"authorizer\"", cpk[cpv->k_id].k);
                    gw_plugin_config_free(gw);
                    return HANDLER_ERROR;
                }
                cpv->v.v = gw;
                cpv->vtype = T_CONFIG_LOCAL;
                break;
              case 1: /* wstunnel.balance */
                cpv->v.u = (unsigned int)gw_get_defaults_balance(srv, cpv->v.b);
                break;
              case 2: /* wstunnel.debug */
              case 3: /* wstunnel.map-extensions */
                break;
              case 4: /* wstunnel.frame-type */
                /*(default frame-type to "text" unless "binary" is specified)*/
                cpv->v.u =
                  buffer_eq_icase_slen(cpv->v.b, CONST_STR_LEN("binary"));
                break;
              case 5: /* wstunnel.origins */
                for (uint32_t j = 0; j < cpv->v.a->used; ++j) {
                    buffer *origin = &((data_string *)cpv->v.a->data[j])->value;
                    if (buffer_is_blank(origin)) {
                        log_error(srv->errh, __FILE__, __LINE__,
                          "unexpected empty string in %s", cpk[cpv->k_id].k);
                        return HANDLER_ERROR;
                    }
                }
                break;
              case 6: /* wstunnel.ping-interval */
                break;
              default:/* should not happen */
                break;
            }
        }

        /* disable check-local for all exts (default enabled) */
        if (gw && gw->exts) { /*(check after gw_set_defaults_backend())*/
            gw_exts_clear_check_local(gw->exts);
        }
    }

    /* default is 0 */
    /*p->defaults.balance = (unsigned int)gw_get_defaults_balance(srv, NULL);*/
    p->defaults.ping_interval = 0; /* do not send ping */

    /* initialize p->defaults from global config context */
    if (p->nconfig > 0 && p->cvlist->v.u2[1]) {
        const config_plugin_value_t *cpv = p->cvlist + p->cvlist->v.u2[0];
        if (-1 != cpv->k_id)
            mod_wstunnel_merge_config(&p->defaults, cpv);
    }

    return HANDLER_GO_ON;
}