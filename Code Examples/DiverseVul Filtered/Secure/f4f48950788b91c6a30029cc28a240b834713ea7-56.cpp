static void calculate_rotation_params(ASS_Renderer *render_priv, DBBox *bbox,
                                      double device_x, double device_y)
{
    TextInfo *text_info = &render_priv->text_info;
    DVector center;
    int i;

    if (render_priv->state.have_origin) {
        center.x = x2scr(render_priv, render_priv->state.org_x);
        center.y = y2scr(render_priv, render_priv->state.org_y);
    } else {
        double bx = 0., by = 0.;
        get_base_point(bbox, render_priv->state.alignment, &bx, &by);
        center.x = device_x + bx;
        center.y = device_y + by;
    }

    for (i = 0; i < text_info->length; ++i) {
        GlyphInfo *info = text_info->glyphs + i;
        while (info) {
            OutlineBitmapHashKey *key = &info->hash_key.u.outline;

            if (key->frx || key->fry || key->frz || key->fax || key->fay) {
                key->shift_x = info->pos.x + double_to_d6(device_x - center.x);
                key->shift_y = -(info->pos.y + double_to_d6(device_y - center.y));
            } else {
                key->shift_x = 0;
                key->shift_y = 0;
            }
            info = info->next;
        }
    }
}