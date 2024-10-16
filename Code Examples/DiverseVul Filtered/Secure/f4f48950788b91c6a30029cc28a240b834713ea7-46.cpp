static ASS_Style *handle_selective_style_overrides(ASS_Renderer *render_priv,
                                                   ASS_Style *rstyle)
{
    // The script style is the one the event was declared with.
    ASS_Style *script = render_priv->track->styles +
                        render_priv->state.event->Style;
    // The user style was set with ass_set_selective_style_override().
    ASS_Style *user = &render_priv->user_override_style;
    ASS_Style *new = &render_priv->state.override_style_temp_storage;
    int explicit = event_has_hard_overrides(render_priv->state.event->Text) ||
                   render_priv->state.evt_type != EVENT_NORMAL;
    int requested = render_priv->settings.selective_style_overrides;
    double scale;

    user->Name = "OverrideStyle"; // name insignificant

    // Either the event's style, or the style forced with a \r tag.
    if (!rstyle)
        rstyle = script;

    // Create a new style that contains a mix of the original style and
    // user_style (the user's override style). Copy only fields from the
    // script's style that are deemed necessary.
    *new = *rstyle;

    render_priv->state.explicit = explicit;

    render_priv->state.apply_font_scale =
        !explicit || !(requested & ASS_OVERRIDE_BIT_SELECTIVE_FONT_SCALE);

    // On positioned events, do not apply most overrides.
    if (explicit)
        requested = 0;

    if (requested & ASS_OVERRIDE_BIT_STYLE)
        requested |= ASS_OVERRIDE_BIT_FONT_NAME |
                     ASS_OVERRIDE_BIT_FONT_SIZE_FIELDS |
                     ASS_OVERRIDE_BIT_COLORS |
                     ASS_OVERRIDE_BIT_BORDER |
                     ASS_OVERRIDE_BIT_ATTRIBUTES;

    // Copies fields even not covered by any of the other bits.
    if (requested & ASS_OVERRIDE_FULL_STYLE)
        *new = *user;

    // The user style is supposed to be independent of the script resolution.
    // Treat the user style's values as if they were specified for a script with
    // PlayResY=288, and rescale the values to the current script.
    scale = render_priv->track->PlayResY / 288.0;

    if (requested & ASS_OVERRIDE_BIT_FONT_SIZE_FIELDS) {
        new->FontSize = user->FontSize * scale;
        new->Spacing = user->Spacing * scale;
        new->ScaleX = user->ScaleX;
        new->ScaleY = user->ScaleY;
    }

    if (requested & ASS_OVERRIDE_BIT_FONT_NAME) {
        new->FontName = user->FontName;
        new->treat_fontname_as_pattern = user->treat_fontname_as_pattern;
    }

    if (requested & ASS_OVERRIDE_BIT_COLORS) {
        new->PrimaryColour = user->PrimaryColour;
        new->SecondaryColour = user->SecondaryColour;
        new->OutlineColour = user->OutlineColour;
        new->BackColour = user->BackColour;
    }

    if (requested & ASS_OVERRIDE_BIT_ATTRIBUTES) {
        new->Bold = user->Bold;
        new->Italic = user->Italic;
        new->Underline = user->Underline;
        new->StrikeOut = user->StrikeOut;
    }

    if (requested & ASS_OVERRIDE_BIT_BORDER) {
        new->BorderStyle = user->BorderStyle;
        new->Outline = user->Outline * scale;
        new->Shadow = user->Shadow * scale;
    }

    if (requested & ASS_OVERRIDE_BIT_ALIGNMENT)
        new->Alignment = user->Alignment;

    if (requested & ASS_OVERRIDE_BIT_MARGINS) {
        new->MarginL = user->MarginL;
        new->MarginR = user->MarginR;
        new->MarginV = user->MarginV;
    }

    if (!new->FontName)
        new->FontName = rstyle->FontName;

    render_priv->state.style = new;
    render_priv->state.overrides = requested;

    return new;
}