kadm5_ret_t krb5_copy_key_data_contents(context, from, to)
    krb5_context context;
    krb5_key_data *from, *to;
{
    int i, idx;

    *to = *from;

    idx = (from->key_data_ver == 1 ? 1 : 2);

    for (i = 0; i < idx; i++) {
        if ( from->key_data_length[i] ) {
            to->key_data_contents[i] = malloc(from->key_data_length[i]);
            if (to->key_data_contents[i] == NULL) {
                for (i = 0; i < idx; i++) {
                    if (to->key_data_contents[i]) {
                        memset(to->key_data_contents[i], 0,
                               to->key_data_length[i]);
                        free(to->key_data_contents[i]);
                    }
                }
                return ENOMEM;
            }
            memcpy(to->key_data_contents[i], from->key_data_contents[i],
                   from->key_data_length[i]);
        }
    }
    return 0;
}