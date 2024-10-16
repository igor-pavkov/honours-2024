static void init_font_scale(ASS_Renderer *render_priv)
{
    ASS_Settings *settings_priv = &render_priv->settings;

    render_priv->font_scale = ((double) render_priv->orig_height) /
                              render_priv->track->PlayResY;
    if (settings_priv->storage_height)
        render_priv->blur_scale = ((double) render_priv->orig_height) /
            settings_priv->storage_height;
    else
        render_priv->blur_scale = 1.;
    if (render_priv->track->ScaledBorderAndShadow)
        render_priv->border_scale =
            ((double) render_priv->orig_height) /
            render_priv->track->PlayResY;
    else
        render_priv->border_scale = render_priv->blur_scale;
    if (!settings_priv->storage_height)
        render_priv->blur_scale = render_priv->border_scale;

    if (render_priv->state.apply_font_scale) {
        render_priv->font_scale *= settings_priv->font_size_coeff;
        render_priv->border_scale *= settings_priv->font_size_coeff;
        render_priv->blur_scale *= settings_priv->font_size_coeff;
    }
}