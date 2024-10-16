static const char *set_nc_check(cmd_parms *cmd, void *config, int flag)
{
#if !APR_HAS_SHARED_MEMORY
    if (flag) {
        return "AuthDigestNcCheck: ERROR: nonce-count checking "
                     "is not supported on platforms without shared-memory "
                     "support";
    }
#endif

    ((digest_config_rec *) config)->check_nc = flag;
    return NULL;
}