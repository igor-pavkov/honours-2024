static double x2scr_pos(ASS_Renderer *render_priv, double x)
{
    return x * render_priv->orig_width / render_priv->font_scale_x / render_priv->track->PlayResX +
        render_priv->settings.left_margin;
}