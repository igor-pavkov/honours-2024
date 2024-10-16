static void oss_enable_out(HWVoiceOut *hw, bool enable)
{
    int trig;
    OSSVoiceOut *oss = (OSSVoiceOut *) hw;
    AudiodevOssPerDirectionOptions *opdo = oss->dev->u.oss.out;

    if (enable) {
        bool poll_mode = opdo->try_poll;

        ldebug("enabling voice\n");
        if (poll_mode) {
            oss_poll_out(hw);
            poll_mode = 0;
        }
        hw->poll_mode = poll_mode;

        if (!oss->mmapped) {
            return;
        }

        audio_pcm_info_clear_buf(&hw->info, hw->buf_emul, hw->mix_buf->size);
        trig = PCM_ENABLE_OUTPUT;
        if (ioctl(oss->fd, SNDCTL_DSP_SETTRIGGER, &trig) < 0) {
            oss_logerr(errno,
                       "SNDCTL_DSP_SETTRIGGER PCM_ENABLE_OUTPUT failed\n");
            return;
        }
    } else {
        if (hw->poll_mode) {
            qemu_set_fd_handler (oss->fd, NULL, NULL, NULL);
            hw->poll_mode = 0;
        }

        if (!oss->mmapped) {
            return;
        }

        ldebug ("disabling voice\n");
        trig = 0;
        if (ioctl (oss->fd, SNDCTL_DSP_SETTRIGGER, &trig) < 0) {
            oss_logerr (errno, "SNDCTL_DSP_SETTRIGGER 0 failed\n");
            return;
        }
    }
}