static void gx_ttfReader__Seek(ttfReader *self, int nPos)
{
    gx_ttfReader *r = (gx_ttfReader *)self;

    r->pos = nPos;
}