void reset_render_context(ASS_Renderer *render_priv, ASS_Style *style)
{
    style = handle_selective_style_overrides(render_priv, style);

    init_font_scale(render_priv);

    render_priv->state.c[0] = style->PrimaryColour;
    render_priv->state.c[1] = style->SecondaryColour;
    render_priv->state.c[2] = style->OutlineColour;
    render_priv->state.c[3] = style->BackColour;
    render_priv->state.flags =
        (style->Underline ? DECO_UNDERLINE : 0) |
        (style->StrikeOut ? DECO_STRIKETHROUGH : 0);
    render_priv->state.font_size = style->FontSize;

    free(render_priv->state.family);
    render_priv->state.family = NULL;
    render_priv->state.family = strdup(style->FontName);
    render_priv->state.treat_family_as_pattern =
        style->treat_fontname_as_pattern;
    render_priv->state.bold = style->Bold;
    render_priv->state.italic = style->Italic;
    update_font(render_priv);

    render_priv->state.border_style = style->BorderStyle;
    render_priv->state.border_x = style->Outline;
    render_priv->state.border_y = style->Outline;
    change_border(render_priv, render_priv->state.border_x, render_priv->state.border_y);
    render_priv->state.scale_x = style->ScaleX;
    render_priv->state.scale_y = style->ScaleY;
    render_priv->state.hspacing = style->Spacing;
    render_priv->state.be = 0;
    render_priv->state.blur = style->Blur;
    render_priv->state.shadow_x = style->Shadow;
    render_priv->state.shadow_y = style->Shadow;
    render_priv->state.frx = render_priv->state.fry = 0.;
    render_priv->state.frz = M_PI * style->Angle / 180.;
    render_priv->state.fax = render_priv->state.fay = 0.;
    render_priv->state.font_encoding = style->Encoding;
}