TPM_RESULT SWTPM_NVRAM_Init(void)
{
    const char  *backend_uri;
    TPM_RESULT  rc = 0;
    TPM_DEBUG(" SWTPM_NVRAM_Init:\n");

    backend_uri = tpmstate_get_backend_uri();
    if (!backend_uri) {
        logprintf(STDERR_FILENO,
                  "SWTPM_NVRAM_Init: Missing backend URI.\n");
        rc = TPM_FAIL;
    } else if (strncmp(backend_uri, "dir://", 6) == 0) {
        g_nvram_backend_ops = &nvram_dir_ops;
    } else if (strncmp(backend_uri, "file://", 7) == 0) {
        g_nvram_backend_ops = &nvram_linear_ops;
    } else {
        logprintf(STDERR_FILENO,
                  "SWTPM_NVRAM_Init: Unsupported backend.\n");
        rc = TPM_FAIL;
    }

    if (rc == 0)
        rc = g_nvram_backend_ops->prepare(backend_uri);

    return rc;
}