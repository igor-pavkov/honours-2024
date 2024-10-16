static void add_background(ASS_Renderer *render_priv, EventImages *event_images)
{
    void *nbuffer = ass_aligned_alloc(1, event_images->width * event_images->height, false);
    if (!nbuffer)
        return;
    memset(nbuffer, 0xFF, event_images->width * event_images->height);
    ASS_Image *img = my_draw_bitmap(nbuffer, event_images->width,
                                    event_images->height,
                                    event_images->width,
                                    event_images->left,
                                    event_images->top,
                                    render_priv->state.c[3], NULL);
    if (img) {
        img->next = event_images->imgs;
        event_images->imgs = img;
    }
}