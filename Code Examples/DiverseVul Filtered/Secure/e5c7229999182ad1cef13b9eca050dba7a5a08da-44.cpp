static enum AVCodecID remap_deprecated_codec_id(enum AVCodecID id)
{
    switch(id){
        //This is for future deprecatec codec ids, its empty since
        //last major bump but will fill up again over time, please don't remove it
//         case AV_CODEC_ID_UTVIDEO_DEPRECATED: return AV_CODEC_ID_UTVIDEO;
        case AV_CODEC_ID_OPUS_DEPRECATED: return AV_CODEC_ID_OPUS;
        case AV_CODEC_ID_TAK_DEPRECATED : return AV_CODEC_ID_TAK;
        case AV_CODEC_ID_PCM_S24LE_PLANAR_DEPRECATED : return AV_CODEC_ID_PCM_S24LE_PLANAR;
        case AV_CODEC_ID_PCM_S32LE_PLANAR_DEPRECATED : return AV_CODEC_ID_PCM_S32LE_PLANAR;
        case AV_CODEC_ID_ESCAPE130_DEPRECATED : return AV_CODEC_ID_ESCAPE130;
        case AV_CODEC_ID_G2M_DEPRECATED : return AV_CODEC_ID_G2M;
        case AV_CODEC_ID_WEBP_DEPRECATED: return AV_CODEC_ID_WEBP;
        case AV_CODEC_ID_HEVC_DEPRECATED: return AV_CODEC_ID_HEVC;
        default                         : return id;
    }
}