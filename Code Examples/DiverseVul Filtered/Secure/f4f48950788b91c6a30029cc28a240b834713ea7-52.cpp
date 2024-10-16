ASS_Image *ass_render_frame(ASS_Renderer *priv, ASS_Track *track,
                            long long now, int *detect_change)
{
    int i, cnt, rc;
    EventImages *last;
    ASS_Image **tail;

    // init frame
    rc = ass_start_frame(priv, track, now);
    if (rc != 0) {
        if (detect_change) {
            *detect_change = 2;
        }
        return NULL;
    }

    // render events separately
    cnt = 0;
    for (i = 0; i < track->n_events; ++i) {
        ASS_Event *event = track->events + i;
        if ((event->Start <= now)
            && (now < (event->Start + event->Duration))) {
            if (cnt >= priv->eimg_size) {
                priv->eimg_size += 100;
                priv->eimg =
                    realloc(priv->eimg,
                            priv->eimg_size * sizeof(EventImages));
            }
            rc = ass_render_event(priv, event, priv->eimg + cnt);
            if (!rc)
                ++cnt;
        }
    }

    // sort by layer
    qsort(priv->eimg, cnt, sizeof(EventImages), cmp_event_layer);

    // call fix_collisions for each group of events with the same layer
    last = priv->eimg;
    for (i = 1; i < cnt; ++i)
        if (last->event->Layer != priv->eimg[i].event->Layer) {
            fix_collisions(priv, last, priv->eimg + i - last);
            last = priv->eimg + i;
        }
    if (cnt > 0)
        fix_collisions(priv, last, priv->eimg + cnt - last);

    // concat lists
    tail = &priv->images_root;
    for (i = 0; i < cnt; ++i) {
        ASS_Image *cur = priv->eimg[i].imgs;
        while (cur) {
            *tail = cur;
            tail = &cur->next;
            cur = cur->next;
        }
    }
    ass_frame_ref(priv->images_root);

    if (detect_change)
        *detect_change = ass_detect_change(priv);

    // free the previous image list
    ass_frame_unref(priv->prev_images_root);
    priv->prev_images_root = NULL;

    return priv->images_root;
}