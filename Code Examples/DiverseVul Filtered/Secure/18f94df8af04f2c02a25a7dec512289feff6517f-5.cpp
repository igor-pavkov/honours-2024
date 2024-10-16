static void parcor_to_lpc(unsigned int k, const int32_t *par, int32_t *cof)
{
    int i, j;

    for (i = 0, j = k - 1; i < j; i++, j--) {
        int tmp1 = ((MUL64(par[k], cof[j]) + (1 << 19)) >> 20);
        cof[j]  += ((MUL64(par[k], cof[i]) + (1 << 19)) >> 20);
        cof[i]  += tmp1;
    }
    if (i == j)
        cof[i] += ((MUL64(par[k], cof[j]) + (1 << 19)) >> 20);

    cof[k] = par[k];
}