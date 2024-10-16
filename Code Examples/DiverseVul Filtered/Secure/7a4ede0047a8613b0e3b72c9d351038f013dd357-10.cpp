static int oss_init_in(HWVoiceIn *hw, struct audsettings *as, void *drv_opaque)
{
    OSSVoiceIn *oss = (OSSVoiceIn *) hw;
    struct oss_params req, obt;
    int endianness;
    int err;
    int fd;
    AudioFormat effective_fmt;
    struct audsettings obt_as;
    Audiodev *dev = drv_opaque;

    oss->fd = -1;

    req.fmt = aud_to_ossfmt (as->fmt, as->endianness);
    req.freq = as->freq;
    req.nchannels = as->nchannels;
    if (oss_open(1, &req, as, &obt, &fd, dev)) {
        return -1;
    }

    err = oss_to_audfmt (obt.fmt, &effective_fmt, &endianness);
    if (err) {
        oss_anal_close (&fd);
        return -1;
    }

    obt_as.freq = obt.freq;
    obt_as.nchannels = obt.nchannels;
    obt_as.fmt = effective_fmt;
    obt_as.endianness = endianness;

    audio_pcm_init_info (&hw->info, &obt_as);
    oss->nfrags = obt.nfrags;
    oss->fragsize = obt.fragsize;

    if (obt.nfrags * obt.fragsize % hw->info.bytes_per_frame) {
        dolog ("warning: Misaligned ADC buffer, size %d, alignment %d\n",
               obt.nfrags * obt.fragsize, hw->info.bytes_per_frame);
    }

    hw->samples = (obt.nfrags * obt.fragsize) / hw->info.bytes_per_frame;

    oss->fd = fd;
    oss->dev = dev;
    return 0;
}