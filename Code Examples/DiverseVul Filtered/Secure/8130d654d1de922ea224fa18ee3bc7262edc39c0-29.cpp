int X509_subject_name_cmp(const X509 *a, const X509 *b)
{
    return X509_NAME_cmp(a->cert_info.subject, b->cert_info.subject);
}