static void free_keys(krb5_context context, krb5_kdcpreauth_rock rock,
                      krb5_keyblock *keys)
{
    krb5_keyblock *k;

    if (keys == NULL)
        return;
    for (k = keys; k->enctype != 0; k++)
        krb5_free_keyblock_contents(context, k);
    free(keys);
}