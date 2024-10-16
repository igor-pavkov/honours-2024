TPM_RESULT SWTPM_NVRAM_SetStateBlob(unsigned char *data,
                                    uint32_t length,
                                    TPM_BOOL is_encrypted,
                                    uint32_t tpm_number SWTPM_ATTR_UNUSED,
                                    uint32_t blobtype)
{
    TPM_RESULT res;
    uint32_t dataoffset;
    unsigned char *plain = NULL, *mig_decrypt = NULL;
    uint32_t plain_len = 0, mig_decrypt_len = 0;
    uint16_t hdrflags;
    enum TPMLIB_StateType st = tpmlib_blobtype_to_statetype(blobtype);
    const char *blobname = tpmlib_get_blobname(blobtype);
    uint8_t hdrversion;

    if (st == 0) {
        logprintf(STDERR_FILENO,
                  "Unknown blob type %u\n", blobtype);
        return TPM_BAD_PARAMETER;
    }

    if (length == 0)
        return TPMLIB_SetState(st, NULL, 0);

    res = SWTPM_NVRAM_CheckHeader(data, length, &dataoffset, &hdrflags,
                                  &hdrversion, false);
    if (res != TPM_SUCCESS)
        return res;

    if (length - dataoffset == 0)
        return TPMLIB_SetState(st, NULL, 0);

    /*
     * We allow setting of blobs that were not encrypted before;
     * we just will not decrypt them even if the migration key is
     * set. This allows to 'upgrade' to encryption. 'Downgrading'
     * will not be possible once a migration key was used.
     */
    if ((hdrflags & BLOB_FLAG_MIGRATION_ENCRYPTED)) {
        /*
         * we first need to decrypt the data with the migration key
         */
        if (!SWTPM_NVRAM_Has_MigrationKey()) {
            logprintf(STDERR_FILENO,
                      "Missing migration key to decrypt %s\n", blobname);
            return TPM_KEYNOTFOUND;
        }

        res = SWTPM_NVRAM_DecryptData(&migrationkey,
                                      &mig_decrypt, &mig_decrypt_len,
                                      &data[dataoffset], length - dataoffset,
                                      TAG_ENCRYPTED_MIGRATION_DATA,
                                      hdrversion,
                                      TAG_IVEC_ENCRYPTED_MIGRATION_DATA,
                                      hdrflags, BLOB_FLAG_MIGRATION_256BIT_KEY);
        if (res) {
            logprintf(STDERR_FILENO,
                      "Decrypting the %s blob with the migration key failed; "
                      "res = %d\n", blobname, res);
            return res;
        }
    } else {
        res = SWTPM_NVRAM_GetPlainData(&mig_decrypt, &mig_decrypt_len,
                                       &data[dataoffset], length - dataoffset,
                                       TAG_MIGRATION_DATA,
                                       hdrversion);
        if (res)
            return res;
    }

    /*
     * Migration key has decrytped the data; if they are still encrypted
     * with the state encryption key, we need to decrypt them using that
     * key now.
     */
    if (is_encrypted || (hdrflags & BLOB_FLAG_ENCRYPTED)) {
        if (!SWTPM_NVRAM_Has_FileKey()) {
            logprintf(STDERR_FILENO,
                      "Missing state key to decrypt %s\n", blobname);
            res = TPM_KEYNOTFOUND;
            goto cleanup;
        }

        res = SWTPM_NVRAM_DecryptData(&filekey, &plain, &plain_len,
                                      mig_decrypt, mig_decrypt_len,
                                      TAG_ENCRYPTED_DATA,
                                      hdrversion, TAG_IVEC_ENCRYPTED_DATA,
                                      hdrflags, BLOB_FLAG_ENCRYPTED_256BIT_KEY);
        if (res) {
            logprintf(STDERR_FILENO,
                      "Decrypting the %s blob with the state key "
                      "failed; res = %d\n", blobname, res);
            goto cleanup;
        }
    } else {
        res = SWTPM_NVRAM_GetPlainData(&plain, &plain_len,
                                       mig_decrypt, mig_decrypt_len,
                                       TAG_DATA,
                                       hdrversion);
        if (res)
            goto cleanup;
    }

    /* SetState will make a copy of the buffer */
    res = TPMLIB_SetState(st, plain, plain_len);

    free(plain);

cleanup:
    free(mig_decrypt);

    return res;
}