static void *oss_get_buffer_out(HWVoiceOut *hw, size_t *size)
{
    OSSVoiceOut *oss = (OSSVoiceOut *) hw;
    if (oss->mmapped) {
        *size = MIN(oss_get_available_bytes(oss), hw->size_emul - hw->pos_emul);
        return hw->buf_emul + hw->pos_emul;
    } else {
        return audio_generic_get_buffer_out(hw, size);
    }
}