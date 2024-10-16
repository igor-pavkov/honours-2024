krb5_error_code ipadb_get_ipapwd_policy(struct ipadb_context *ipactx,
                                        char *pw_policy_dn,
                                        struct ipapwd_policy **_pol)
{
    struct ipapwd_policy *pol;
    krb5_error_code kerr;
    LDAPMessage *res = NULL;
    LDAPMessage *lentry;
    uint32_t result;
    int ret;

    pol = calloc(1, sizeof(struct ipapwd_policy));
    if (!pol) {
        return ENOMEM;
    }

    pol->max_pwd_life = IPAPWD_DEFAULT_PWDLIFE;
    pol->min_pwd_length = IPAPWD_DEFAULT_MINLEN;

    kerr = ipadb_simple_search(ipactx, pw_policy_dn, LDAP_SCOPE_BASE,
                               "(objectClass=*)", std_pwdpolicy_attrs, &res);
    if (kerr) {
        goto done;
    }

    lentry = ldap_first_entry(ipactx->lcontext, res);
    if (!lentry) {
        kerr = KRB5_KDB_INTERNAL_ERROR;
        goto done;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbMinPwdLife", &result);
    if (ret == 0) {
        pol->min_pwd_life = result;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbMaxPwdLife", &result);
    if (ret == 0) {
        pol->max_pwd_life = result;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbPwdMinLength", &result);
    if (ret == 0) {
        pol->min_pwd_length = result;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbPwdHistoryLength", &result);
    if (ret == 0) {
        pol->history_length = result;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbPwdMinDiffChars", &result);
    if (ret == 0) {
        pol->min_complexity = result;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbPwdMaxFailure", &result);
    if (ret == 0) {
        pol->max_fail = result;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbPwdFailureCountInterval", &result);
    if (ret == 0) {
        pol->failcnt_interval = result;
    }

    ret = ipadb_ldap_attr_to_uint32(ipactx->lcontext, lentry,
                                    "krbPwdLockoutDuration", &result);
    if (ret == 0) {
        pol->lockout_duration = result;
    }

    *_pol = pol;

done:
    ldap_msgfree(res);
    if (kerr) {
        free(pol);
    }
    return kerr;
}