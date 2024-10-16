int BN_bn2binpad(const BIGNUM *a, unsigned char *to, int tolen)
{
    if (tolen < 0)
        return -1;
    return bn2binpad(a, to, tolen);
}