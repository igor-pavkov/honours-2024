static size_t oss_put_buffer_out(HWVoiceOut *hw, void *buf, size_t size)
{
    OSSVoiceOut *oss = (OSSVoiceOut *) hw;
    if (oss->mmapped) {
        assert(buf == hw->buf_emul + hw->pos_emul && size < hw->size_emul);

        hw->pos_emul = (hw->pos_emul + size) % hw->size_emul;
        return size;
    } else {
        return audio_generic_put_buffer_out(hw, buf, size);
    }
}