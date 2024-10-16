static PROXY_CERT_INFO_EXTENSION *r2i_pci(X509V3_EXT_METHOD *method,
                                          X509V3_CTX *ctx, char *value)
{
    PROXY_CERT_INFO_EXTENSION *pci = NULL;
    STACK_OF(CONF_VALUE) *vals;
    ASN1_OBJECT *language = NULL;
    ASN1_INTEGER *pathlen = NULL;
    ASN1_OCTET_STRING *policy = NULL;
    int i, j;

    vals = X509V3_parse_list(value);
    for (i = 0; i < sk_CONF_VALUE_num(vals); i++) {
        CONF_VALUE *cnf = sk_CONF_VALUE_value(vals, i);
        if (!cnf->name || (*cnf->name != '@' && !cnf->value)) {
            X509V3err(X509V3_F_R2I_PCI,
                      X509V3_R_INVALID_PROXY_POLICY_SETTING);
            X509V3_conf_err(cnf);
            goto err;
        }
        if (*cnf->name == '@') {
            STACK_OF(CONF_VALUE) *sect;
            int success_p = 1;

            sect = X509V3_get_section(ctx, cnf->name + 1);
            if (!sect) {
                X509V3err(X509V3_F_R2I_PCI, X509V3_R_INVALID_SECTION);
                X509V3_conf_err(cnf);
                goto err;
            }
            for (j = 0; success_p && j < sk_CONF_VALUE_num(sect); j++) {
                success_p =
                    process_pci_value(sk_CONF_VALUE_value(sect, j),
                                      &language, &pathlen, &policy);
            }
            X509V3_section_free(ctx, sect);
            if (!success_p)
                goto err;
        } else {
            if (!process_pci_value(cnf, &language, &pathlen, &policy)) {
                X509V3_conf_err(cnf);
                goto err;
            }
        }
    }

    /* Language is mandatory */
    if (!language) {
        X509V3err(X509V3_F_R2I_PCI,
                  X509V3_R_NO_PROXY_CERT_POLICY_LANGUAGE_DEFINED);
        goto err;
    }
    i = OBJ_obj2nid(language);
    if ((i == NID_Independent || i == NID_id_ppl_inheritAll) && policy) {
        X509V3err(X509V3_F_R2I_PCI,
                  X509V3_R_POLICY_WHEN_PROXY_LANGUAGE_REQUIRES_NO_POLICY);
        goto err;
    }

    pci = PROXY_CERT_INFO_EXTENSION_new();
    if (pci == NULL) {
        X509V3err(X509V3_F_R2I_PCI, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    pci->proxyPolicy->policyLanguage = language;
    language = NULL;
    pci->proxyPolicy->policy = policy;
    policy = NULL;
    pci->pcPathLengthConstraint = pathlen;
    pathlen = NULL;
    goto end;
 err:
    ASN1_OBJECT_free(language);
    ASN1_INTEGER_free(pathlen);
    pathlen = NULL;
    ASN1_OCTET_STRING_free(policy);
    policy = NULL;
    PROXY_CERT_INFO_EXTENSION_free(pci);
    pci = NULL;
 end:
    sk_CONF_VALUE_pop_free(vals, X509V3_conf_free);
    return pci;
}