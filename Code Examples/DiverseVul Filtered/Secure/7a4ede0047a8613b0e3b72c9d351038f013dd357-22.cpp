static size_t oss_read(HWVoiceIn *hw, void *buf, size_t len)
{
    OSSVoiceIn *oss = (OSSVoiceIn *) hw;
    size_t pos = 0;

    while (len) {
        ssize_t nread;

        void *dst = advance(buf, pos);
        nread = read(oss->fd, dst, len);

        if (nread == -1) {
            switch (errno) {
            case EINTR:
            case EAGAIN:
                break;
            default:
                oss_logerr(errno, "Failed to read %zu bytes of audio (to %p)\n",
                           len, dst);
                break;
            }
        }

        pos += nread;
        len -= nread;
    }

    return pos;
}