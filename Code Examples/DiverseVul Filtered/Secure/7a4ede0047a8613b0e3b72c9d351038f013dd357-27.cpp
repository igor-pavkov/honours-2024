static size_t oss_get_available_bytes(OSSVoiceOut *oss)
{
    int err;
    struct count_info cntinfo;
    assert(oss->mmapped);

    err = ioctl(oss->fd, SNDCTL_DSP_GETOPTR, &cntinfo);
    if (err < 0) {
        oss_logerr(errno, "SNDCTL_DSP_GETOPTR failed\n");
        return 0;
    }

    return audio_ring_dist(cntinfo.ptr, oss->hw.pos_emul, oss->hw.size_emul);
}