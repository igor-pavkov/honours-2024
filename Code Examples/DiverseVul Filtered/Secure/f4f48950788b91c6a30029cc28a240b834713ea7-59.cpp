static double x2scr(ASS_Renderer *render_priv, double x)
{
    if (render_priv->state.explicit)
        return x2scr_pos(render_priv, x);
    return x * render_priv->orig_width_nocrop / render_priv->font_scale_x /
        render_priv->track->PlayResX +
        FFMAX(render_priv->settings.left_margin, 0);
}