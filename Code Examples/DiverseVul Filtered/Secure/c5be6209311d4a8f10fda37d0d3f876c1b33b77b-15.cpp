kadm5_ret_t kadm5_decrypt_key(void *server_handle,
                              kadm5_principal_ent_t entry, krb5_int32
                              ktype, krb5_int32 stype, krb5_int32
                              kvno, krb5_keyblock *keyblock,
                              krb5_keysalt *keysalt, int *kvnop)
{
    kadm5_server_handle_t handle = server_handle;
    krb5_db_entry dbent;
    krb5_key_data *key_data;
    krb5_keyblock *mkey_ptr;
    int ret;

    CHECK_HANDLE(server_handle);

    if (entry->n_key_data == 0 || entry->key_data == NULL)
        return EINVAL;

    /* find_enctype only uses these two fields */
    dbent.n_key_data = entry->n_key_data;
    dbent.key_data = entry->key_data;
    if ((ret = krb5_dbe_find_enctype(handle->context, &dbent, ktype,
                                     stype, kvno, &key_data)))
        return ret;

    /* find_mkey only uses this field */
    dbent.tl_data = entry->tl_data;
    if ((ret = krb5_dbe_find_mkey(handle->context, &dbent, &mkey_ptr))) {
        /* try refreshing master key list */
        /* XXX it would nice if we had the mkvno here for optimization */
        if (krb5_db_fetch_mkey_list(handle->context, master_princ,
                                    &master_keyblock) == 0) {
            if ((ret = krb5_dbe_find_mkey(handle->context, &dbent,
                                          &mkey_ptr))) {
                return ret;
            }
        } else {
            return ret;
        }
    }

    if ((ret = krb5_dbe_decrypt_key_data(handle->context, NULL, key_data,
                                         keyblock, keysalt)))
        return ret;

    /*
     * Coerce the enctype of the output keyblock in case we got an
     * inexact match on the enctype; this behavior will go away when
     * the key storage architecture gets redesigned for 1.3.
     */
    if (ktype != -1)
        keyblock->enctype = ktype;

    if (kvnop)
        *kvnop = key_data->key_data_kvno;

    return KADM5_OK;
}