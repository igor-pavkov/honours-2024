static void render_and_combine_glyphs(ASS_Renderer *render_priv,
                                      double device_x, double device_y)
{
    TextInfo *text_info = &render_priv->text_info;
    int left = render_priv->settings.left_margin;
    device_x = (device_x - left) * render_priv->font_scale_x + left;
    unsigned nb_bitmaps = 0;
    char linebreak = 0;
    CombinedBitmapInfo *combined_info = text_info->combined_bitmaps;
    CombinedBitmapInfo *current_info = NULL;
    GlyphInfo *last_info = NULL;
    for (int i = 0; i < text_info->length; i++) {
        GlyphInfo *info = text_info->glyphs + i;
        if (info->linebreak) linebreak = 1;
        if (info->skip) {
            for (; info; info = info->next)
                ass_cache_dec_ref(info->hash_key.u.outline.outline);
            continue;
        }
        for (; info; info = info->next) {
            OutlineBitmapHashKey *key = &info->hash_key.u.outline;

            info->pos.x = double_to_d6(device_x + d6_to_double(info->pos.x) * render_priv->font_scale_x);
            info->pos.y = double_to_d6(device_y) + info->pos.y;
            key->advance.x = info->pos.x & (SUBPIXEL_MASK & ~SUBPIXEL_ACCURACY);
            key->advance.y = info->pos.y & (SUBPIXEL_MASK & ~SUBPIXEL_ACCURACY);
            int x = info->pos.x >> 6, y = info->pos.y >> 6;
            get_bitmap_glyph(render_priv, info);

            if(linebreak || is_new_bm_run(info, last_info)) {
                linebreak = 0;
                last_info = NULL;
                if (nb_bitmaps >= text_info->max_bitmaps) {
                    size_t new_size = 2 * text_info->max_bitmaps;
                    if (!ASS_REALLOC_ARRAY(text_info->combined_bitmaps, new_size)) {
                        ass_cache_dec_ref(info->image);
                        continue;
                    }
                    text_info->max_bitmaps = new_size;
                    combined_info = text_info->combined_bitmaps;
                }
                current_info = &combined_info[nb_bitmaps];

                memcpy(&current_info->c, &info->c, sizeof(info->c));
                current_info->effect_type = info->effect_type;
                current_info->effect_timing = info->effect_timing;
                current_info->first_pos_x = info->bbox.xMax >> 6;

                current_info->filter.flags = 0;
                if (info->border_style == 3)
                    current_info->filter.flags |= FILTER_BORDER_STYLE_3;
                if (info->border_x || info->border_y)
                    current_info->filter.flags |= FILTER_NONZERO_BORDER;
                if (info->shadow_x || info->shadow_y)
                    current_info->filter.flags |= FILTER_NONZERO_SHADOW;
                // VSFilter compatibility: invisible fill and no border?
                // In this case no shadow is supposed to be rendered.
                if (info->border || (info->c[0] & 0xFF) != 0xFF)
                    current_info->filter.flags |= FILTER_DRAW_SHADOW;

                current_info->filter.be = info->be;
                current_info->filter.blur = 2 * info->blur * render_priv->blur_scale;
                current_info->filter.shadow.x = double_to_d6(info->shadow_x * render_priv->border_scale);
                current_info->filter.shadow.y = double_to_d6(info->shadow_y * render_priv->border_scale);

                current_info->x = current_info->y = INT_MAX;
                rectangle_reset(&current_info->rect);
                rectangle_reset(&current_info->rect_o);
                current_info->n_bm = current_info->n_bm_o = 0;
                current_info->bm = current_info->bm_o = current_info->bm_s = NULL;
                current_info->image = NULL;

                current_info->bitmap_count = current_info->max_bitmap_count = 0;
                current_info->bitmaps = malloc(MAX_SUB_BITMAPS_INITIAL * sizeof(BitmapRef));
                if (!current_info->bitmaps) {
                    ass_cache_dec_ref(info->image);
                    continue;
                }
                current_info->max_bitmap_count = MAX_SUB_BITMAPS_INITIAL;

                nb_bitmaps++;
            }
            last_info = info;

            if (!info->image || !current_info) {
                ass_cache_dec_ref(info->image);
                continue;
            }

            if (current_info->bitmap_count >= current_info->max_bitmap_count) {
                size_t new_size = 2 * current_info->max_bitmap_count;
                if (!ASS_REALLOC_ARRAY(current_info->bitmaps, new_size)) {
                    ass_cache_dec_ref(info->image);
                    continue;
                }
                current_info->max_bitmap_count = new_size;
            }
            current_info->bitmaps[current_info->bitmap_count].image = info->image;
            current_info->bitmaps[current_info->bitmap_count].x = x;
            current_info->bitmaps[current_info->bitmap_count].y = y;
            current_info->bitmap_count++;

            current_info->x = FFMIN(current_info->x, x);
            current_info->y = FFMIN(current_info->y, y);
            if (info->image->bm) {
                rectangle_combine(&current_info->rect, info->image->bm, x, y);
                current_info->n_bm++;
            }
            if (info->image->bm_o) {
                rectangle_combine(&current_info->rect_o, info->image->bm_o, x, y);
                current_info->n_bm_o++;
            }
        }
    }

    for (int i = 0; i < nb_bitmaps; i++) {
        CombinedBitmapInfo *info = &combined_info[i];
        for (int j = 0; j < info->bitmap_count; j++) {
            info->bitmaps[j].x -= info->x;
            info->bitmaps[j].y -= info->y;
        }

        CompositeHashKey hk;
        CompositeHashValue *hv;
        fill_composite_hash(&hk, info);
        if (ass_cache_get(render_priv->cache.composite_cache, &hk, &hv)) {
            info->bm = hv->bm;
            info->bm_o = hv->bm_o;
            info->bm_s = hv->bm_s;
            info->image = hv;
            continue;
        }
        if (!hv)
            continue;

        int bord = be_padding(info->filter.be);
        if (!bord && info->n_bm == 1) {
            for (int j = 0; j < info->bitmap_count; j++) {
                if (!info->bitmaps[j].image->bm)
                    continue;
                info->bm = copy_bitmap(render_priv->engine, info->bitmaps[j].image->bm);
                if (info->bm) {
                    info->bm->left += info->bitmaps[j].x;
                    info->bm->top  += info->bitmaps[j].y;
                }
                break;
            }
        } else if (info->n_bm) {
            info->bm = alloc_bitmap(render_priv->engine,
                                    info->rect.x_max - info->rect.x_min + 2 * bord,
                                    info->rect.y_max - info->rect.y_min + 2 * bord, true);
            Bitmap *dst = info->bm;
            if (dst) {
                dst->left = info->rect.x_min - info->x - bord;
                dst->top  = info->rect.y_min - info->y - bord;
                for (int j = 0; j < info->bitmap_count; j++) {
                    Bitmap *src = info->bitmaps[j].image->bm;
                    if (!src)
                        continue;
                    int x = info->bitmaps[j].x + src->left - dst->left;
                    int y = info->bitmaps[j].y + src->top  - dst->top;
                    assert(x >= 0 && x + src->w <= dst->w);
                    assert(y >= 0 && y + src->h <= dst->h);
                    unsigned char *buf = dst->buffer + y * dst->stride + x;
                    render_priv->engine->add_bitmaps(buf, dst->stride,
                                                     src->buffer, src->stride,
                                                     src->h, src->w);
                }
            }
        }
        if (!bord && info->n_bm_o == 1) {
            for (int j = 0; j < info->bitmap_count; j++) {
                if (!info->bitmaps[j].image->bm_o)
                    continue;
                info->bm_o = copy_bitmap(render_priv->engine, info->bitmaps[j].image->bm_o);
                if (info->bm_o) {
                    info->bm_o->left += info->bitmaps[j].x;
                    info->bm_o->top  += info->bitmaps[j].y;
                }
                break;
            }
        } else if (info->n_bm_o) {
            info->bm_o = alloc_bitmap(render_priv->engine,
                                      info->rect_o.x_max - info->rect_o.x_min + 2 * bord,
                                      info->rect_o.y_max - info->rect_o.y_min + 2 * bord,
                                      true);
            Bitmap *dst = info->bm_o;
            if (dst) {
                dst->left = info->rect_o.x_min - info->x - bord;
                dst->top  = info->rect_o.y_min - info->y - bord;
                for (int j = 0; j < info->bitmap_count; j++) {
                    Bitmap *src = info->bitmaps[j].image->bm_o;
                    if (!src)
                        continue;
                    int x = info->bitmaps[j].x + src->left - dst->left;
                    int y = info->bitmaps[j].y + src->top  - dst->top;
                    assert(x >= 0 && x + src->w <= dst->w);
                    assert(y >= 0 && y + src->h <= dst->h);
                    unsigned char *buf = dst->buffer + y * dst->stride + x;
                    render_priv->engine->add_bitmaps(buf, dst->stride,
                                                     src->buffer, src->stride,
                                                     src->h, src->w);
                }
            }
        }

        if (info->bm || info->bm_o) {
            ass_synth_blur(render_priv->engine, info->filter.flags & FILTER_BORDER_STYLE_3,
                           info->filter.be, info->filter.blur, info->bm, info->bm_o);
            if (info->filter.flags & FILTER_DRAW_SHADOW)
                make_shadow_bitmap(info, render_priv);
        }

        hv->bm = info->bm;
        hv->bm_o = info->bm_o;
        hv->bm_s = info->bm_s;
        ass_cache_commit(hv, bitmap_size(hv->bm) +
                         bitmap_size(hv->bm_o) + bitmap_size(hv->bm_s) +
                         sizeof(CompositeHashKey) + sizeof(CompositeHashValue));
        info->image = hv;
    }

    text_info->n_bitmaps = nb_bitmaps;
}