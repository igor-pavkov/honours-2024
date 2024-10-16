unsigned avcodec_version(void)
{
//    av_assert0(AV_CODEC_ID_V410==164);
    av_assert0(AV_CODEC_ID_PCM_S8_PLANAR==65563);
    av_assert0(AV_CODEC_ID_ADPCM_G722==69660);
//     av_assert0(AV_CODEC_ID_BMV_AUDIO==86071);
    av_assert0(AV_CODEC_ID_SRT==94216);
    av_assert0(LIBAVCODEC_VERSION_MICRO >= 100);

    av_assert0(CODEC_ID_CLLC == AV_CODEC_ID_CLLC);
    av_assert0(CODEC_ID_PCM_S8_PLANAR == AV_CODEC_ID_PCM_S8_PLANAR);
    av_assert0(CODEC_ID_ADPCM_IMA_APC == AV_CODEC_ID_ADPCM_IMA_APC);
    av_assert0(CODEC_ID_ILBC == AV_CODEC_ID_ILBC);
    av_assert0(CODEC_ID_SRT == AV_CODEC_ID_SRT);
    return LIBAVCODEC_VERSION_INT;
}