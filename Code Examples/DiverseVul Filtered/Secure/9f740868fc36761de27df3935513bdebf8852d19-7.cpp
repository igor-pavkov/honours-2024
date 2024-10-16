int SWTPM_NVRAM_PrintJson(void)
{
    TPM_RESULT rc = 0;
    const char *backend_uri;
    const char *states[] = {
        TPM_PERMANENT_ALL_NAME,
        TPM_VOLATILESTATE_NAME,
        TPM_SAVESTATE_NAME,
    };
    char state_str[200] = "";
    size_t i, n, o, blobsize;
    int ret = -1;

    rc = SWTPM_NVRAM_Init();
    if (rc == 0) {
        o = 0;
        backend_uri = tpmstate_get_backend_uri();

        for (i = 0; i < ARRAY_LEN(states); i++) {
            rc = g_nvram_backend_ops->check_state(backend_uri, states[i],
                                                  &blobsize);
            if (rc == TPM_SUCCESS) {
                n = snprintf(&state_str[o], sizeof(state_str) - o,
                             "%s {\"name\": \"%s\", \"size\": %zu}",
                             (o > 0) ? "," : "",
                             states[i], blobsize);
                if (n >= sizeof(state_str) - o)
                    goto exit;
                o += n;
            } else if (rc != TPM_RETRY) {
                /* Error other than ENOENT */
                goto exit;
            }
        }
        printf("{ \"type\": \"swtpm\", \"states\": [%s%s] }",
               state_str,  (o > 0) ? " ": "");
        ret = 0;
    }

exit:
    return ret;
}