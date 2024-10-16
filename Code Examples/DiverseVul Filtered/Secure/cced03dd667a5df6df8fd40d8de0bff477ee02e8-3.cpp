int ff_mms_read_header(MMSContext *mms, uint8_t *buf, const int size)
{
    char *pos;
    int size_to_copy;
    int remaining_size = mms->asf_header_size - mms->asf_header_read_size;
    size_to_copy = FFMIN(size, remaining_size);
    pos = mms->asf_header + mms->asf_header_read_size;
    memcpy(buf, pos, size_to_copy);
    if (mms->asf_header_read_size == mms->asf_header_size) {
        av_freep(&mms->asf_header); // which contains asf header
    }
    mms->asf_header_read_size += size_to_copy;
    return size_to_copy;
}