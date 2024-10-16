void free_history_entry(krb5_context context, osa_pw_hist_ent *hist)
{
    int i;

    for (i = 0; i < hist->n_key_data; i++)
        krb5_free_key_data_contents(context, &hist->key_data[i]);
    free(hist->key_data);
}