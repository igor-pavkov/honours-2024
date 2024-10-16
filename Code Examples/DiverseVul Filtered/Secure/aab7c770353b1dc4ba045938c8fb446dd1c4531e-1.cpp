void bn_init(BIGNUM *a)
{
    static BIGNUM nilbn;

    *a = nilbn;
    bn_check_top(a);
}