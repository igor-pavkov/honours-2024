static void oss_helper_poll_in (void *opaque)
{
    AudioState *s = opaque;
    audio_run(s, "oss_poll_in");
}