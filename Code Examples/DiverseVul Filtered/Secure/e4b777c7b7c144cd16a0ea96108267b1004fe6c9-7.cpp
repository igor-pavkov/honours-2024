gif_result gif_decode_frame(gif_animation *gif, unsigned int frame)
{
        return gif_internal_decode_frame(gif, frame, false);
}