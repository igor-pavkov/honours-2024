STACK_OF(CONF_VALUE) *X509V3_parse_list(const char *line)
{
    char *p, *q, c;
    char *ntmp, *vtmp;
    STACK_OF(CONF_VALUE) *values = NULL;
    char *linebuf;
    int state;
    /* We are going to modify the line so copy it first */
    linebuf = OPENSSL_strdup(line);
    if (linebuf == NULL) {
        X509V3err(X509V3_F_X509V3_PARSE_LIST, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    state = HDR_NAME;
    ntmp = NULL;
    /* Go through all characters */
    for (p = linebuf, q = linebuf; (c = *p) && (c != '\r') && (c != '\n');
         p++) {

        switch (state) {
        case HDR_NAME:
            if (c == ':') {
                state = HDR_VALUE;
                *p = 0;
                ntmp = strip_spaces(q);
                if (!ntmp) {
                    X509V3err(X509V3_F_X509V3_PARSE_LIST,
                              X509V3_R_INVALID_NULL_NAME);
                    goto err;
                }
                q = p + 1;
            } else if (c == ',') {
                *p = 0;
                ntmp = strip_spaces(q);
                q = p + 1;
                if (!ntmp) {
                    X509V3err(X509V3_F_X509V3_PARSE_LIST,
                              X509V3_R_INVALID_NULL_NAME);
                    goto err;
                }
                X509V3_add_value(ntmp, NULL, &values);
            }
            break;

        case HDR_VALUE:
            if (c == ',') {
                state = HDR_NAME;
                *p = 0;
                vtmp = strip_spaces(q);
                if (!vtmp) {
                    X509V3err(X509V3_F_X509V3_PARSE_LIST,
                              X509V3_R_INVALID_NULL_VALUE);
                    goto err;
                }
                X509V3_add_value(ntmp, vtmp, &values);
                ntmp = NULL;
                q = p + 1;
            }

        }
    }

    if (state == HDR_VALUE) {
        vtmp = strip_spaces(q);
        if (!vtmp) {
            X509V3err(X509V3_F_X509V3_PARSE_LIST,
                      X509V3_R_INVALID_NULL_VALUE);
            goto err;
        }
        X509V3_add_value(ntmp, vtmp, &values);
    } else {
        ntmp = strip_spaces(q);
        if (!ntmp) {
            X509V3err(X509V3_F_X509V3_PARSE_LIST, X509V3_R_INVALID_NULL_NAME);
            goto err;
        }
        X509V3_add_value(ntmp, NULL, &values);
    }
    OPENSSL_free(linebuf);
    return values;

 err:
    OPENSSL_free(linebuf);
    sk_CONF_VALUE_pop_free(values, X509V3_conf_free);
    return NULL;

}