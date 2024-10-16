unsigned long X509_subject_name_hash(X509 *x)
{
    return X509_NAME_hash_ex(x->cert_info.subject, NULL, NULL, NULL);
}