int BN_bn2bin(const BIGNUM *a, unsigned char *to)
{
    return bn2binpad(a, to, -1);
}