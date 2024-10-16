TPM_RESULT SWTPM_NVRAM_DeleteName(uint32_t tpm_number,
                                  const char *name,
                                  TPM_BOOL mustExist)
{
    const char *backend_uri = NULL;
    backend_uri = tpmstate_get_backend_uri();

    return g_nvram_backend_ops->delete(tpm_number, name, mustExist,
                                       backend_uri);
}