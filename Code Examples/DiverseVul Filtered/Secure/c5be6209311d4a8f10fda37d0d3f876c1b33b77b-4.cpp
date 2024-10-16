static krb5_tl_data *dup_tl_data(krb5_tl_data *tl)
{
    krb5_tl_data *n;

    n = (krb5_tl_data *) malloc(sizeof(krb5_tl_data));
    if (n == NULL)
        return NULL;
    n->tl_data_contents = malloc(tl->tl_data_length);
    if (n->tl_data_contents == NULL) {
        free(n);
        return NULL;
    }
    memcpy(n->tl_data_contents, tl->tl_data_contents, tl->tl_data_length);
    n->tl_data_type = tl->tl_data_type;
    n->tl_data_length = tl->tl_data_length;
    n->tl_data_next = NULL;
    return n;
}