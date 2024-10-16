static double y2scr_pos(ASS_Renderer *render_priv, double y)
{
    return y * render_priv->orig_height / render_priv->track->PlayResY +
        render_priv->settings.top_margin;
}