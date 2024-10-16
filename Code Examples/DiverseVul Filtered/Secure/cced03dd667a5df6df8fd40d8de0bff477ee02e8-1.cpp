int ff_mms_read_data(MMSContext *mms, uint8_t *buf, const int size)
{
    int read_size;
    read_size = FFMIN(size, mms->remaining_in_len);
    memcpy(buf, mms->read_in_ptr, read_size);
    mms->remaining_in_len -= read_size;
    mms->read_in_ptr      += read_size;
    return read_size;
}