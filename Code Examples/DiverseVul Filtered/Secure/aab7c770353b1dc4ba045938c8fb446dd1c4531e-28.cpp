void BN_GENCB_set_old(BN_GENCB *gencb, void (*callback) (int, int, void *),
                      void *cb_arg)
{
    BN_GENCB *tmp_gencb = gencb;
    tmp_gencb->ver = 1;
    tmp_gencb->arg = cb_arg;
    tmp_gencb->cb.cb_1 = callback;
}