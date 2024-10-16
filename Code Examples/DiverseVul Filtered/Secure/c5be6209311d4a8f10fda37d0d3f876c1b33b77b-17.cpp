static int decrypt_key_data(krb5_context context,
                            int n_key_data, krb5_key_data *key_data,
                            krb5_keyblock **keyblocks, int *n_keys)
{
    krb5_keyblock *keys;
    int ret, i;

    keys = (krb5_keyblock *) malloc(n_key_data*sizeof(krb5_keyblock));
    if (keys == NULL)
        return ENOMEM;
    memset(keys, 0, n_key_data*sizeof(krb5_keyblock));

    for (i = 0; i < n_key_data; i++) {
        ret = krb5_dbe_decrypt_key_data(context, NULL, &key_data[i], &keys[i],
                                        NULL);
        if (ret) {
            for (; i >= 0; i--) {
                if (keys[i].contents) {
                    memset (keys[i].contents, 0, keys[i].length);
                    free( keys[i].contents );
                }
            }

            memset(keys, 0, n_key_data*sizeof(krb5_keyblock));
            free(keys);
            return ret;
        }
    }

    *keyblocks = keys;
    if (n_keys)
        *n_keys = n_key_data;

    return 0;
}