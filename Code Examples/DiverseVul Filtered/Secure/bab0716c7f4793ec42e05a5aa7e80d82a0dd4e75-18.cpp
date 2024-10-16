static int klv_read_packet(KLVPacket *klv, AVIOContext *pb)
{
    int64_t length, pos;
    if (!mxf_read_sync(pb, mxf_klv_key, 4))
        return AVERROR_INVALIDDATA;
    klv->offset = avio_tell(pb) - 4;
    memcpy(klv->key, mxf_klv_key, 4);
    avio_read(pb, klv->key + 4, 12);
    length = klv_decode_ber_length(pb);
    if (length < 0)
        return length;
    klv->length = length;
    pos = avio_tell(pb);
    if (pos > INT64_MAX - length)
        return AVERROR_INVALIDDATA;
    klv->next_klv = pos + length;
    return 0;
}