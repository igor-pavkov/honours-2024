int ff_lock_avcodec(AVCodecContext *log_ctx)
{
    if (lockmgr_cb) {
        if ((*lockmgr_cb)(&codec_mutex, AV_LOCK_OBTAIN))
            return -1;
    }
    entangled_thread_counter++;
    if (entangled_thread_counter != 1) {
        av_log(log_ctx, AV_LOG_ERROR, "Insufficient thread locking around avcodec_open/close()\n");
        if (!lockmgr_cb)
            av_log(log_ctx, AV_LOG_ERROR, "No lock manager is set, please see av_lockmgr_register()\n");
        ff_avcodec_locked = 1;
        ff_unlock_avcodec();
        return AVERROR(EINVAL);
    }
    av_assert0(!ff_avcodec_locked);
    ff_avcodec_locked = 1;
    return 0;
}