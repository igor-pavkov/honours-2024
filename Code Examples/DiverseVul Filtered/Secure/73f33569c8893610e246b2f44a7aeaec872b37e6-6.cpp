krb5_error_code ipadb_check_policy_as(krb5_context kcontext,
                                      krb5_kdc_req *request,
                                      krb5_db_entry *client,
                                      krb5_db_entry *server,
                                      krb5_timestamp kdc_time,
                                      const char **status,
                                      krb5_pa_data ***e_data)
{
    struct ipadb_context *ipactx;
    struct ipadb_e_data *ied;
    krb5_error_code kerr;

    if (!client) {
        return ENOENT;
    }

    ipactx = ipadb_get_context(kcontext);
    if (!ipactx) {
        return EINVAL;
    }

    ied = (struct ipadb_e_data *)client->e_data;
    if (!ied) {
        return EINVAL;
    }

    if (!ied->pol) {
        kerr = ipadb_get_ipapwd_policy(ipactx, ied->pw_policy_dn, &ied->pol);
        if (kerr != 0) {
            return kerr;
        }
    }

    if (client->last_failed <= ied->last_admin_unlock) {
        /* admin unlocked the account */
        return 0;
    }

    if (ied->pol->max_fail == 0 ||
        client->fail_auth_count < ied->pol->max_fail) {
        /* still within allowed failures range */
        return 0;
    }

    if (ied->pol->lockout_duration == 0 ||
        client->last_failed + ied->pol->lockout_duration > kdc_time) {
        /* ok client permanently locked, or within lockout period */
        *status = "LOCKED_OUT";
        return KRB5KDC_ERR_CLIENT_REVOKED;
    }

    return 0;
}