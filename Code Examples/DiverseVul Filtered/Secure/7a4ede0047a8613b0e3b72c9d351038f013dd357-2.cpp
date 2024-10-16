static void oss_enable_in(HWVoiceIn *hw, bool enable)
{
    OSSVoiceIn *oss = (OSSVoiceIn *) hw;
    AudiodevOssPerDirectionOptions *opdo = oss->dev->u.oss.out;

    if (enable) {
        bool poll_mode = opdo->try_poll;

        if (poll_mode) {
            oss_poll_in(hw);
            poll_mode = 0;
        }
        hw->poll_mode = poll_mode;
    } else {
        if (hw->poll_mode) {
            hw->poll_mode = 0;
            qemu_set_fd_handler (oss->fd, NULL, NULL, NULL);
        }
    }
}