static int x509_subject_cmp(X509 **a, X509 **b)
{
    return X509_subject_name_cmp(*a, *b);
}