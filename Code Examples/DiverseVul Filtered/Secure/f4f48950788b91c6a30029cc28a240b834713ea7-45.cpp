static void stroke_outline(ASS_Renderer *render_priv, ASS_Outline *outline,
                           int sx, int sy)
{
    if (sx <= 0 && sy <= 0)
        return;

    fix_freetype_stroker(outline, sx, sy);

    size_t n_points = outline->n_points;
    if (n_points > SHRT_MAX) {
        ass_msg(render_priv->library, MSGL_WARN, "Too many outline points: %d",
                outline->n_points);
        n_points = SHRT_MAX;
    }

    size_t n_contours = FFMIN(outline->n_contours, SHRT_MAX);
    short contours_small[EFFICIENT_CONTOUR_COUNT];
    short *contours = contours_small;
    short *contours_large = NULL;
    if (n_contours > EFFICIENT_CONTOUR_COUNT) {
        contours_large = malloc(n_contours * sizeof(short));
        if (!contours_large)
            return;
        contours = contours_large;
    }
    for (size_t i = 0; i < n_contours; ++i)
        contours[i] = FFMIN(outline->contours[i], n_points - 1);

    FT_Outline ftol;
    ftol.n_points = n_points;
    ftol.n_contours = n_contours;
    ftol.points = outline->points;
    ftol.tags = outline->tags;
    ftol.contours = contours;
    ftol.flags = 0;

    // Borders are equal; use the regular stroker
    if (sx == sy && render_priv->state.stroker) {
        int error;
        FT_StrokerBorder border = FT_Outline_GetOutsideBorder(&ftol);
        error = FT_Stroker_ParseOutline(render_priv->state.stroker, &ftol, 0);
        if (error) {
            ass_msg(render_priv->library, MSGL_WARN,
                    "FT_Stroker_ParseOutline failed, error: %d", error);
        }
        unsigned new_points, new_contours;
        error = FT_Stroker_GetBorderCounts(render_priv->state.stroker, border,
                &new_points, &new_contours);
        if (error) {
            ass_msg(render_priv->library, MSGL_WARN,
                    "FT_Stroker_GetBorderCounts failed, error: %d", error);
        }
        outline_free(outline);
        outline->n_points = outline->n_contours = 0;
        if (new_contours > FFMAX(EFFICIENT_CONTOUR_COUNT, n_contours)) {
            if (!ASS_REALLOC_ARRAY(contours_large, new_contours)) {
                free(contours_large);
                return;
            }
        }
        n_points = new_points;
        n_contours = new_contours;
        if (!outline_alloc(outline, n_points, n_contours)) {
            ass_msg(render_priv->library, MSGL_WARN,
                    "Not enough memory for border outline");
            free(contours_large);
            return;
        }
        ftol.n_points = ftol.n_contours = 0;
        ftol.points = outline->points;
        ftol.tags = outline->tags;

        FT_Stroker_ExportBorder(render_priv->state.stroker, border, &ftol);

        outline->n_points = n_points;
        outline->n_contours = n_contours;
        for (size_t i = 0; i < n_contours; ++i)
            outline->contours[i] = (unsigned short) contours[i];

    // "Stroke" with the outline emboldener (in two passes if needed).
    // The outlines look uglier, but the emboldening never adds any points
    } else {
#if (FREETYPE_MAJOR > 2) || \
    ((FREETYPE_MAJOR == 2) && (FREETYPE_MINOR > 4)) || \
    ((FREETYPE_MAJOR == 2) && (FREETYPE_MINOR == 4) && (FREETYPE_PATCH >= 10))
        FT_Outline_EmboldenXY(&ftol, sx * 2, sy * 2);
        FT_Outline_Translate(&ftol, -sx, -sy);
#else
        int i;
        FT_Outline nol;

        FT_Outline_New(render_priv->ftlibrary, ftol.n_points,
                       ftol.n_contours, &nol);
        FT_Outline_Copy(&ftol, &nol);

        FT_Outline_Embolden(&ftol, sx * 2);
        FT_Outline_Translate(&ftol, -sx, -sx);
        FT_Outline_Embolden(&nol, sy * 2);
        FT_Outline_Translate(&nol, -sy, -sy);

        for (i = 0; i < ftol.n_points; i++)
            ftol.points[i].y = nol.points[i].y;

        FT_Outline_Done(render_priv->ftlibrary, &nol);
#endif
    }

    free(contours_large);
}