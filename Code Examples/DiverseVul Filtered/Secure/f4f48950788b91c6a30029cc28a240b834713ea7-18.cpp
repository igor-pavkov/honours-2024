static void blend_vector_clip(ASS_Renderer *render_priv,
                              ASS_Image *head)
{
    ASS_Drawing *drawing = render_priv->state.clip_drawing;
    if (!drawing)
        return;

    // Try to get mask from cache
    BitmapHashKey key;
    memset(&key, 0, sizeof(key));
    key.type = BITMAP_CLIP;
    key.u.clip.text = drawing->text;

    BitmapHashValue *val;
    if (!ass_cache_get(render_priv->cache.bitmap_cache, &key, &val)) {
        if (!val)
            return;
        val->bm = val->bm_o = NULL;

        // Not found in cache, parse and rasterize it
        ASS_Outline *outline = ass_drawing_parse(drawing, 1);
        if (!outline) {
            ass_msg(render_priv->library, MSGL_WARN,
                    "Clip vector parsing failed. Skipping.");
            ass_cache_commit(val, sizeof(BitmapHashKey) + sizeof(BitmapHashValue));
            ass_cache_dec_ref(val);
            return;
        }

        // We need to translate the clip according to screen borders
        if (render_priv->settings.left_margin != 0 ||
            render_priv->settings.top_margin != 0) {
            FT_Vector trans = {
                .x = int_to_d6(render_priv->settings.left_margin),
                .y = -int_to_d6(render_priv->settings.top_margin),
            };
            outline_translate(outline, trans.x, trans.y);
        }

        val->bm = outline_to_bitmap(render_priv, outline, 0);
        ass_cache_commit(val, bitmap_size(val->bm) +
                         sizeof(BitmapHashKey) + sizeof(BitmapHashValue));
    }

    Bitmap *clip_bm = val->bm;
    if (!clip_bm) {
        ass_cache_dec_ref(val);
        return;
    }

    // Iterate through bitmaps and blend/clip them
    for (ASS_Image *cur = head; cur; cur = cur->next) {
        int left, top, right, bottom, w, h;
        int ax, ay, aw, ah, as;
        int bx, by, bw, bh, bs;
        int aleft, atop, bleft, btop;
        unsigned char *abuffer, *bbuffer, *nbuffer;

        render_priv->state.has_clips = 1;

        abuffer = cur->bitmap;
        bbuffer = clip_bm->buffer;
        ax = cur->dst_x;
        ay = cur->dst_y;
        aw = cur->w;
        ah = cur->h;
        as = cur->stride;
        bx = clip_bm->left;
        by = clip_bm->top;
        bw = clip_bm->w;
        bh = clip_bm->h;
        bs = clip_bm->stride;

        // Calculate overlap coordinates
        left = (ax > bx) ? ax : bx;
        top = (ay > by) ? ay : by;
        right = ((ax + aw) < (bx + bw)) ? (ax + aw) : (bx + bw);
        bottom = ((ay + ah) < (by + bh)) ? (ay + ah) : (by + bh);
        aleft = left - ax;
        atop = top - ay;
        w = right - left;
        h = bottom - top;
        bleft = left - bx;
        btop = top - by;

        if (render_priv->state.clip_drawing_mode) {
            // Inverse clip
            if (ax + aw < bx || ay + ah < by || ax > bx + bw ||
                ay > by + bh || !h || !w) {
                continue;
            }

            // Allocate new buffer and add to free list
            nbuffer = ass_aligned_alloc(32, as * ah, false);
            if (!nbuffer)
                break;

            // Blend together
            memcpy(nbuffer, abuffer, ((ah - 1) * as) + aw);
            render_priv->engine->sub_bitmaps(nbuffer + atop * as + aleft, as,
                                             bbuffer + btop * bs + bleft, bs,
                                             h, w);
        } else {
            // Regular clip
            if (ax + aw < bx || ay + ah < by || ax > bx + bw ||
                ay > by + bh || !h || !w) {
                cur->w = cur->h = cur->stride = 0;
                continue;
            }

            // Allocate new buffer and add to free list
            unsigned align = (w >= 16) ? 16 : ((w >= 8) ? 8 : 1);
            unsigned ns = ass_align(align, w);
            nbuffer = ass_aligned_alloc(align, ns * h, false);
            if (!nbuffer)
                break;

            // Blend together
            render_priv->engine->mul_bitmaps(nbuffer, ns,
                                             abuffer + atop * as + aleft, as,
                                             bbuffer + btop * bs + bleft, bs,
                                             w, h);
            cur->dst_x += aleft;
            cur->dst_y += atop;
            cur->w = w;
            cur->h = h;
            cur->stride = ns;
        }

        cur->bitmap = nbuffer;
        ASS_ImagePriv *priv = (ASS_ImagePriv *) cur;
        ass_cache_dec_ref(priv->source);
        priv->source = NULL;
    }

    ass_cache_dec_ref(val);
}