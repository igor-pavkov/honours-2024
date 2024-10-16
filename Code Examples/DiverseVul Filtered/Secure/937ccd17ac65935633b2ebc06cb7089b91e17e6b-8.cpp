static void gx_ttfExport__Close(ttfExport *self)
{
    gx_ttfExport *e = (gx_ttfExport *)self;

    if (!e->error)
        e->error = gx_path_close_subpath_notes(e->path, sn_none);
}