int am_check_permissions(request_rec *r, am_cache_entry_t *session)
{
    am_dir_cfg_rec *dir_cfg;
    int i, j;
    int skip_or = 0;
    const apr_array_header_t *backrefs = NULL;

    dir_cfg = am_get_dir_cfg(r);

    /* Iterate over all cond-directives */
    for (i = 0; i < dir_cfg->cond->nelts; i++) {
        const am_cond_t *ce;
        const char *value = NULL;
        int match = 0;

        ce = &((am_cond_t *)(dir_cfg->cond->elts))[i];

        am_diag_printf(r, "%s processing condition %d of %d: %s ",
                       __func__, i, dir_cfg->cond->nelts,
                       am_diag_cond_str(r, ce));

        /*
         * Rule with ignore flag?
         */
        if (ce->flags & AM_COND_FLAG_IGN)
            continue;

        /* 
         * We matched a [OR] rule, skip the next rules
         * until we have one without [OR]. 
         */
        if (skip_or) {
            if (!(ce->flags & AM_COND_FLAG_OR))
                skip_or = 0;

            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                          "Skip %s, [OR] rule matched previously",
                          ce->directive);

            am_diag_printf(r, "Skip, [OR] rule matched previously\n");
            continue;
        }
        
        /* 
         * look for a match on each value for this attribute, 
         * stop on first match.
         */
        for (j = 0; (j < session->size) && !match; j++) {
            const char *varname = NULL;
            am_envattr_conf_t *envattr_conf = NULL;

            /*
             * if MAP flag is set, check for remapped 
             * attribute name with mellonSetEnv
             */
            if (ce->flags & AM_COND_FLAG_MAP) {
                envattr_conf =  (am_envattr_conf_t *)apr_hash_get(dir_cfg->envattr, 
                                         am_cache_entry_get_string(session,&session->env[j].varname),
                                         APR_HASH_KEY_STRING);
                                                    
                if (envattr_conf != NULL)
                    varname = envattr_conf->name;
            }

            /*
             * Otherwise or if not found, use the attribute name
             * sent by the IdP.
             */
            if (varname == NULL)
                varname = am_cache_entry_get_string(session,
                                                    &session->env[j].varname);
                      
            if (strcmp(varname, ce->varname) != 0)
                    continue;

            value = am_cache_entry_get_string(session, &session->env[j].value);

            /*
             * Substitute backrefs if available
             */
            if (ce->flags & AM_COND_FLAG_FSTR)
                ce = am_cond_substitue(r, ce, backrefs);

            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                          "Evaluate %s vs \"%s\"", 
                          ce->directive, value);

            am_diag_printf(r, "evaluate value \"%s\" ", value);
    
            if (value == NULL) {
                 match = 0;          /* can not happen */

            } else if ((ce->flags & AM_COND_FLAG_REG) && (ce->flags & AM_COND_FLAG_REF)) {
                 int nsub = ce->regex->re_nsub + 1;
                 ap_regmatch_t *regmatch;

                 regmatch = (ap_regmatch_t *)apr_palloc(r->pool, 
                            nsub * sizeof(*regmatch));

                 match = !ap_regexec(ce->regex, value, nsub, regmatch, 0);
                 if (match)
                     backrefs = am_cond_backrefs(r, ce, value, regmatch);

            } else if (ce->flags & AM_COND_FLAG_REG) {
                 match = !ap_regexec(ce->regex, value, 0, NULL, 0);

            } else if ((ce->flags & AM_COND_FLAG_SUB) && (ce->flags & AM_COND_FLAG_NC)) {
                 match = (ap_strcasestr(value, ce->str) != NULL);

            } else if (ce->flags & AM_COND_FLAG_SUB) {
                 match = (strstr(value, ce->str) != NULL);

            } else if (ce->flags & AM_COND_FLAG_NC) {
                 match = !strcasecmp(ce->str, value);

            } else {
                 match = !strcmp(ce->str, value);
            }

            am_diag_printf(r, "match=%s, ", match ? "yes" : "no");
        }

        if (ce->flags & AM_COND_FLAG_NOT) {
            match = !match;

            am_diag_printf(r, "negating now match=%s ", match ? "yes" : "no");
        }

        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                      "%s: %smatch", ce->directive,
                      (match == 0) ? "no ": "");

        /*
         * If no match, we stop here, except if it is an [OR] condition
         */
        if (!match & !(ce->flags & AM_COND_FLAG_OR)) {
            ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r,
                          "Client failed to match %s",
                          ce->directive);

            am_diag_printf(r, "failed (no OR condition)"
                           " returning HTTP_FORBIDDEN\n");
            return HTTP_FORBIDDEN;
        }

        /*
         * Match on [OR] condition means we skip until a rule
         * without [OR], 
         */
        if (match && (ce->flags & AM_COND_FLAG_OR))
            skip_or = 1;

        am_diag_printf(r, "\n");
    }

    am_diag_printf(r, "%s succeeds\n", __func__);

    return OK;
}