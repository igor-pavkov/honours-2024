void gif_finalise(gif_animation *gif)
{
        /* Release all our memory blocks */
        if (gif->frame_image) {
                assert(gif->bitmap_callbacks.bitmap_destroy);
                gif->bitmap_callbacks.bitmap_destroy(gif->frame_image);
        }

        gif->frame_image = NULL;
        free(gif->frames);
        gif->frames = NULL;
        free(gif->local_colour_table);
        gif->local_colour_table = NULL;
        free(gif->global_colour_table);
        gif->global_colour_table = NULL;

        lzw_context_destroy(gif->lzw_ctx);
        gif->lzw_ctx = NULL;
}