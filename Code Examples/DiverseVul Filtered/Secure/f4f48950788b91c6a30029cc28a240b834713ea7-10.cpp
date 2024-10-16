static double y2scr_sub(ASS_Renderer *render_priv, double y)
{
    if (render_priv->state.explicit)
        return y2scr_pos(render_priv, y);
    if (render_priv->settings.use_margins)
        return y * render_priv->orig_height_nocrop /
            render_priv->track->PlayResY +
            FFMAX(render_priv->settings.top_margin, 0)
            + FFMAX(render_priv->settings.bottom_margin, 0);
    else
        return y * render_priv->orig_height_nocrop /
            render_priv->track->PlayResY +
            FFMAX(render_priv->settings.top_margin, 0);
}