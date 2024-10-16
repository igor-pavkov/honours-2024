void BN_GENCB_set(BN_GENCB *gencb, int (*callback) (int, int, BN_GENCB *),
                  void *cb_arg)
{
    BN_GENCB *tmp_gencb = gencb;
    tmp_gencb->ver = 2;
    tmp_gencb->arg = cb_arg;
    tmp_gencb->cb.cb_2 = callback;
}