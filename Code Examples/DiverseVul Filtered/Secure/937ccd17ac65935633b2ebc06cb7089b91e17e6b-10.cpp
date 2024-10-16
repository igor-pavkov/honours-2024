static bool gx_ttfReader__Error(ttfReader *self)
{
    gx_ttfReader *r = (gx_ttfReader *)self;

    return r->error;
}