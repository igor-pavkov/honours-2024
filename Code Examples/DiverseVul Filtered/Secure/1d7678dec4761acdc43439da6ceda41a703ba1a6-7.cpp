int vhdx_user_visible_write(BlockDriverState *bs, BDRVVHDXState *s)
{
    int ret = 0;
    if (s->first_visible_write) {
        s->first_visible_write = false;
        ret = vhdx_update_headers(bs, s, true, NULL);
    }
    return ret;
}