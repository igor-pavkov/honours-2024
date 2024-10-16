static int oss_open(int in, struct oss_params *req, audsettings *as,
                    struct oss_params *obt, int *pfd, Audiodev *dev)
{
    AudiodevOssOptions *oopts = &dev->u.oss;
    AudiodevOssPerDirectionOptions *opdo = in ? oopts->in : oopts->out;
    int fd;
    int oflags = (oopts->has_exclusive && oopts->exclusive) ? O_EXCL : 0;
    audio_buf_info abinfo;
    int fmt, freq, nchannels;
    int setfragment = 1;
    const char *dspname = opdo->has_dev ? opdo->dev : "/dev/dsp";
    const char *typ = in ? "ADC" : "DAC";
#ifdef USE_DSP_POLICY
    int policy = oopts->has_dsp_policy ? oopts->dsp_policy : 5;
#endif

    /* Kludge needed to have working mmap on Linux */
    oflags |= (oopts->has_try_mmap && oopts->try_mmap) ?
        O_RDWR : (in ? O_RDONLY : O_WRONLY);

    fd = open (dspname, oflags | O_NONBLOCK);
    if (-1 == fd) {
        oss_logerr2 (errno, typ, "Failed to open `%s'\n", dspname);
        return -1;
    }

    freq = req->freq;
    nchannels = req->nchannels;
    fmt = req->fmt;
    req->nfrags = opdo->has_buffer_count ? opdo->buffer_count : 4;
    req->fragsize = audio_buffer_bytes(
        qapi_AudiodevOssPerDirectionOptions_base(opdo), as, 23220);

    if (ioctl (fd, SNDCTL_DSP_SAMPLESIZE, &fmt)) {
        oss_logerr2 (errno, typ, "Failed to set sample size %d\n", req->fmt);
        goto err;
    }

    if (ioctl (fd, SNDCTL_DSP_CHANNELS, &nchannels)) {
        oss_logerr2 (errno, typ, "Failed to set number of channels %d\n",
                     req->nchannels);
        goto err;
    }

    if (ioctl (fd, SNDCTL_DSP_SPEED, &freq)) {
        oss_logerr2 (errno, typ, "Failed to set frequency %d\n", req->freq);
        goto err;
    }

    if (ioctl (fd, SNDCTL_DSP_NONBLOCK, NULL)) {
        oss_logerr2 (errno, typ, "Failed to set non-blocking mode\n");
        goto err;
    }

#ifdef USE_DSP_POLICY
    if (policy >= 0) {
        int version;

        if (!oss_get_version (fd, &version, typ)) {
            trace_oss_version(version);

            if (version >= 0x040000) {
                int policy2 = policy;
                if (ioctl(fd, SNDCTL_DSP_POLICY, &policy2)) {
                    oss_logerr2 (errno, typ,
                                 "Failed to set timing policy to %d\n",
                                 policy);
                    goto err;
                }
                setfragment = 0;
            }
        }
    }
#endif

    if (setfragment) {
        int mmmmssss = (req->nfrags << 16) | ctz32 (req->fragsize);
        if (ioctl (fd, SNDCTL_DSP_SETFRAGMENT, &mmmmssss)) {
            oss_logerr2 (errno, typ, "Failed to set buffer length (%d, %d)\n",
                         req->nfrags, req->fragsize);
            goto err;
        }
    }

    if (ioctl (fd, in ? SNDCTL_DSP_GETISPACE : SNDCTL_DSP_GETOSPACE, &abinfo)) {
        oss_logerr2 (errno, typ, "Failed to get buffer length\n");
        goto err;
    }

    if (!abinfo.fragstotal || !abinfo.fragsize) {
        AUD_log (AUDIO_CAP, "Returned bogus buffer information(%d, %d) for %s\n",
                 abinfo.fragstotal, abinfo.fragsize, typ);
        goto err;
    }

    obt->fmt = fmt;
    obt->nchannels = nchannels;
    obt->freq = freq;
    obt->nfrags = abinfo.fragstotal;
    obt->fragsize = abinfo.fragsize;
    *pfd = fd;

#ifdef DEBUG_MISMATCHES
    if ((req->fmt != obt->fmt) ||
        (req->nchannels != obt->nchannels) ||
        (req->freq != obt->freq) ||
        (req->fragsize != obt->fragsize) ||
        (req->nfrags != obt->nfrags)) {
        dolog ("Audio parameters mismatch\n");
        oss_dump_info (req, obt);
    }
#endif

#ifdef DEBUG
    oss_dump_info (req, obt);
#endif
    return 0;

 err:
    oss_anal_close (&fd);
    return -1;
}