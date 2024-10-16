X509_NAME *X509_get_subject_name(const X509 *a)
{
    return a->cert_info.subject;
}