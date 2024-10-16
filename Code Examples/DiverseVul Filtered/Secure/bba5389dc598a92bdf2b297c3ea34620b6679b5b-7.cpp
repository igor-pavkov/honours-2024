static void bs_open_read (Bitstream *bs, void *buffer_start, void *buffer_end)
{
    bs->error = bs->sr = bs->bc = 0;
    bs->ptr = ((bs->buf = (uint16_t *)buffer_start) - 1);
    bs->end = (uint16_t *)buffer_end;
    bs->wrap = bs_read;
}