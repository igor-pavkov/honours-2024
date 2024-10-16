static int gx_ttfReader__Tell(ttfReader *self)
{
    gx_ttfReader *r = (gx_ttfReader *)self;

    return r->pos;
}