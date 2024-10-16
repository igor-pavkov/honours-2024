static void oss_poll_in (HWVoiceIn *hw)
{
    OSSVoiceIn *oss = (OSSVoiceIn *) hw;

    qemu_set_fd_handler(oss->fd, oss_helper_poll_in, NULL, hw->s);
}