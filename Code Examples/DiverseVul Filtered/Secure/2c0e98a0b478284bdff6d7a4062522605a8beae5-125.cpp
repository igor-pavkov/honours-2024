static int mov_pcm_be_gt16(enum AVCodecID codec_id)
{
    return codec_id == AV_CODEC_ID_PCM_S24BE ||
           codec_id == AV_CODEC_ID_PCM_S32BE ||
           codec_id == AV_CODEC_ID_PCM_F32BE ||
           codec_id == AV_CODEC_ID_PCM_F64BE;
}