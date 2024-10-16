static int mov_pcm_le_gt16(enum AVCodecID codec_id)
{
    return codec_id == AV_CODEC_ID_PCM_S24LE ||
           codec_id == AV_CODEC_ID_PCM_S32LE ||
           codec_id == AV_CODEC_ID_PCM_F32LE ||
           codec_id == AV_CODEC_ID_PCM_F64LE;
}