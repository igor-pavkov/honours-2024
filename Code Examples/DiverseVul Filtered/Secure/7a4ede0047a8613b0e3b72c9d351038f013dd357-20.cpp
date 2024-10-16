static void oss_fini_out (HWVoiceOut *hw)
{
    int err;
    OSSVoiceOut *oss = (OSSVoiceOut *) hw;

    ldebug ("oss_fini\n");
    oss_anal_close (&oss->fd);

    if (oss->mmapped && hw->buf_emul) {
        err = munmap(hw->buf_emul, hw->size_emul);
        if (err) {
            oss_logerr(errno, "Failed to unmap buffer %p, size %zu\n",
                       hw->buf_emul, hw->size_emul);
        }
        hw->buf_emul = NULL;
    }
}