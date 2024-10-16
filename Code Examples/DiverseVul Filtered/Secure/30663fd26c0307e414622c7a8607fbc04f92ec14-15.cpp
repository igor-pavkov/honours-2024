static void gen_eob(DisasContext *s)
{
    gen_eob_worker(s, false, false);
}