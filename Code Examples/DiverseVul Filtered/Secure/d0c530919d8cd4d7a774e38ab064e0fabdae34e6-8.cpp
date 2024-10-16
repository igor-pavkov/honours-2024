static mf_t *open_mf_single(void *talloc_ctx, struct mp_log *log, char *filename)
{
    mf_t *mf = talloc_zero(talloc_ctx, mf_t);
    mf->log = log;
    mf_add(mf, filename);
    return mf;
}