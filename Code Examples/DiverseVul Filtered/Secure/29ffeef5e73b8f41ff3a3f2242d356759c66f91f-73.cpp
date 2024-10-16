static void er_add_slice(H264Context *h, int startx, int starty,
                         int endx, int endy, int status)
{
    if (CONFIG_ERROR_RESILIENCE) {
        ERContext *er = &h->er;

        ff_er_add_slice(er, startx, starty, endx, endy, status);
    }
}