static int check_delta_base(X509_CRL *delta, X509_CRL *base)
{
    /* Delta CRL must be a delta */
    if (!delta->base_crl_number)
        return 0;
    /* Base must have a CRL number */
    if (!base->crl_number)
        return 0;
    /* Issuer names must match */
    if (X509_NAME_cmp(X509_CRL_get_issuer(base), X509_CRL_get_issuer(delta)))
        return 0;
    /* AKID and IDP must match */
    if (!crl_extension_match(delta, base, NID_authority_key_identifier))
        return 0;
    if (!crl_extension_match(delta, base, NID_issuing_distribution_point))
        return 0;
    /* Delta CRL base number must not exceed Full CRL number. */
    if (ASN1_INTEGER_cmp(delta->base_crl_number, base->crl_number) > 0)
        return 0;
    /* Delta CRL number must exceed full CRL number */
    if (ASN1_INTEGER_cmp(delta->crl_number, base->crl_number) > 0)
        return 1;
    return 0;
}