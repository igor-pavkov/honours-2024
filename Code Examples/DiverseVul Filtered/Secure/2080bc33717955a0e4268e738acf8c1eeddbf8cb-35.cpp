int av_codec_is_decoder(const AVCodec *codec)
{
    return codec && (codec->decode || codec->send_packet);
}