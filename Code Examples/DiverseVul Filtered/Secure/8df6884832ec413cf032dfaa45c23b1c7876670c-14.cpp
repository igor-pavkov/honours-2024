const AVCodecHWConfig *avcodec_get_hw_config(const AVCodec *codec, int index)
{
    int i;
    if (!codec->hw_configs || index < 0)
        return NULL;
    for (i = 0; i <= index; i++)
        if (!codec->hw_configs[i])
            return NULL;
    return &codec->hw_configs[index]->public;
}