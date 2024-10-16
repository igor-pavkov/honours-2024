void ipadb_free_pwd_policy(krb5_context kcontext, osa_policy_ent_t val)
{
    if (val) {
        free(val->name);
        free(val->allowed_keysalts);
        free(val);
    }
}