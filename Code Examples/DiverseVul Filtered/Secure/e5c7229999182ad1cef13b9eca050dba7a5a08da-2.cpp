int ff_unlock_avcodec(void)
{
    av_assert0(ff_avcodec_locked);
    ff_avcodec_locked = 0;
    entangled_thread_counter--;
    if (lockmgr_cb) {
        if ((*lockmgr_cb)(&codec_mutex, AV_LOCK_RELEASE))
            return -1;
    }
    return 0;
}