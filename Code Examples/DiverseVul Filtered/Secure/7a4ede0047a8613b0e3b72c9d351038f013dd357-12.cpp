static int oss_init_out(HWVoiceOut *hw, struct audsettings *as,
                        void *drv_opaque)
{
    OSSVoiceOut *oss = (OSSVoiceOut *) hw;
    struct oss_params req, obt;
    int endianness;
    int err;
    int fd;
    AudioFormat effective_fmt;
    struct audsettings obt_as;
    Audiodev *dev = drv_opaque;
    AudiodevOssOptions *oopts = &dev->u.oss;

    oss->fd = -1;

    req.fmt = aud_to_ossfmt (as->fmt, as->endianness);
    req.freq = as->freq;
    req.nchannels = as->nchannels;

    if (oss_open(0, &req, as, &obt, &fd, dev)) {
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
        dolog ("warning: Misaligned DAC buffer, size %d, alignment %d\n",
               obt.nfrags * obt.fragsize, hw->info.bytes_per_frame);
    }

    hw->samples = (obt.nfrags * obt.fragsize) / hw->info.bytes_per_frame;

    oss->mmapped = 0;
    if (oopts->has_try_mmap && oopts->try_mmap) {
        hw->size_emul = hw->samples * hw->info.bytes_per_frame;
        hw->buf_emul = mmap(
            NULL,
            hw->size_emul,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd,
            0
            );
        if (hw->buf_emul == MAP_FAILED) {
            oss_logerr(errno, "Failed to map %zu bytes of DAC\n",
                       hw->size_emul);
            hw->buf_emul = NULL;
        } else {
            int err;
            int trig = 0;
            if (ioctl (fd, SNDCTL_DSP_SETTRIGGER, &trig) < 0) {
                oss_logerr (errno, "SNDCTL_DSP_SETTRIGGER 0 failed\n");
            }
            else {
                trig = PCM_ENABLE_OUTPUT;
                if (ioctl (fd, SNDCTL_DSP_SETTRIGGER, &trig) < 0) {
                    oss_logerr (
                        errno,
                        "SNDCTL_DSP_SETTRIGGER PCM_ENABLE_OUTPUT failed\n"
                        );
                }
                else {
                    oss->mmapped = 1;
                }
            }

            if (!oss->mmapped) {
                err = munmap(hw->buf_emul, hw->size_emul);
                if (err) {
                    oss_logerr(errno, "Failed to unmap buffer %p size %zu\n",
                               hw->buf_emul, hw->size_emul);
                }
                hw->buf_emul = NULL;
            }
        }
    }

    oss->fd = fd;
    oss->dev = dev;
    return 0;
}