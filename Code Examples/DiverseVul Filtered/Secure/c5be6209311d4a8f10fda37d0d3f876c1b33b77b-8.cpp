static void cleanup_key_data(context, count, data)
    krb5_context   context;
    int                    count;
    krb5_key_data        * data;
{
    int i, j;

    for (i = 0; i < count; i++)
        for (j = 0; j < data[i].key_data_ver; j++)
            if (data[i].key_data_length[j])
                krb5_db_free(context, data[i].key_data_contents[j]);
    krb5_db_free(context, data);
}