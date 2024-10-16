static MXFWrappingScheme mxf_get_wrapping_kind(UID *essence_container_ul)
{
    int val;
    const MXFCodecUL *codec_ul;

    codec_ul = mxf_get_codec_ul(mxf_picture_essence_container_uls, essence_container_ul);
    if (!codec_ul->uid[0])
        codec_ul = mxf_get_codec_ul(mxf_sound_essence_container_uls, essence_container_ul);
    if (!codec_ul->uid[0])
        codec_ul = mxf_get_codec_ul(mxf_data_essence_container_uls, essence_container_ul);
    if (!codec_ul->uid[0] || !codec_ul->wrapping_indicator_pos)
        return UnknownWrapped;

    val = (*essence_container_ul)[codec_ul->wrapping_indicator_pos];
    switch (codec_ul->wrapping_indicator_type) {
        case RawVWrap:
            val = val % 4;
            break;
        case RawAWrap:
            if (val == 0x03 || val == 0x04)
                val -= 0x02;
            break;
        case D10D11Wrap:
            if (val == 0x02)
                val = 0x01;
            break;
    }
    if (val == 0x01)
        return FrameWrapped;
    if (val == 0x02)
        return ClipWrapped;
    return UnknownWrapped;
}