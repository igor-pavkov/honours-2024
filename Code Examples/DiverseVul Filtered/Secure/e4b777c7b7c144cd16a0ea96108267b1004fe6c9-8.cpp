void gif_create(gif_animation *gif, gif_bitmap_callback_vt *bitmap_callbacks)
{
        memset(gif, 0, sizeof(gif_animation));
        gif->bitmap_callbacks = *bitmap_callbacks;
        gif->decoded_frame = GIF_INVALID_FRAME;
}