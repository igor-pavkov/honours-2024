static void oss_poll_out (HWVoiceOut *hw)
{
    OSSVoiceOut *oss = (OSSVoiceOut *) hw;

    qemu_set_fd_handler(oss->fd, NULL, oss_helper_poll_out, hw->s);
}