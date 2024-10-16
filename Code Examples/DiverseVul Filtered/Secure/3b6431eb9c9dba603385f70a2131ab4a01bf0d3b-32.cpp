static const char *set_algorithm(cmd_parms *cmd, void *config, const char *alg)
{
    if (!strcasecmp(alg, "MD5-sess")) {
        return "AuthDigestAlgorithm: ERROR: algorithm `MD5-sess' "
                "is not implemented";
    }
    else if (strcasecmp(alg, "MD5")) {
        return apr_pstrcat(cmd->pool, "Invalid algorithm in AuthDigestAlgorithm: ", alg, NULL);
    }

    ((digest_config_rec *) config)->algorithm = alg;
    return NULL;
}