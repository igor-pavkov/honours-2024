static void oss_helper_poll_out (void *opaque)
{
    AudioState *s = opaque;
    audio_run(s, "oss_poll_out");
}