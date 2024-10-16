int ttfFont__Open_aux(ttfFont *self, ttfInterpreter *tti, gx_ttfReader *r, gs_font_type42 *pfont,
               const gs_matrix * char_tm, const gs_log2_scale_point *log2_scale,
               bool design_grid)
{
    gs_point char_size, subpix_origin;
    gs_matrix post_transform;
    /*
     * Ghostscript proceses a TTC index in gs/lib/gs_ttf.ps,
     * and *pfont already adjusted to it.
     * Therefore TTC headers never comes here.
     */
    unsigned int nTTC = 0;
    bool dg;

    decompose_matrix(pfont, char_tm, log2_scale, design_grid, &char_size, &subpix_origin, &post_transform, &dg);
    switch(ttfFont__Open(tti, self, &r->super, nTTC, char_size.x, char_size.y, dg)) {
        case fNoError:
            return 0;
        case fMemoryError:
            return_error(gs_error_VMerror);
        case fUnimplemented:
            return_error(gs_error_unregistered);
        case fBadInstruction:
            WarnBadInstruction(pfont, -1);
            goto recover;
        case fPatented:
            WarnPatented(pfont, self, "The font");
        recover:
            self->patented = true;
            return 0;
        default:
            {	int code = r->super.Error(&r->super);

                if (code < 0)
                    return code;
                return_error(gs_error_invalidfont);
            }
    }
}