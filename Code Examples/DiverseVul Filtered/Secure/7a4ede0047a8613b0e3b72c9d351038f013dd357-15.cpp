static void *oss_audio_init(Audiodev *dev)
{
    AudiodevOssOptions *oopts;
    assert(dev->driver == AUDIODEV_DRIVER_OSS);

    oopts = &dev->u.oss;
    oss_init_per_direction(oopts->in);
    oss_init_per_direction(oopts->out);

    if (access(oopts->in->has_dev ? oopts->in->dev : "/dev/dsp",
               R_OK | W_OK) < 0 ||
        access(oopts->out->has_dev ? oopts->out->dev : "/dev/dsp",
               R_OK | W_OK) < 0) {
        return NULL;
    }
    return dev;
}