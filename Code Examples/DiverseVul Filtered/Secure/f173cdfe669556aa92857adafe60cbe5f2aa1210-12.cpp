static void mxf_read_random_index_pack(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    uint32_t length;
    int64_t file_size;
    KLVPacket klv;

    if (!s->pb->seekable)
        return;

    file_size = avio_size(s->pb);
    avio_seek(s->pb, file_size - 4, SEEK_SET);
    length = avio_rb32(s->pb);
    if (length <= 32 || length >= FFMIN(file_size, INT_MAX))
        goto end;
    avio_seek(s->pb, file_size - length, SEEK_SET);
    if (klv_read_packet(&klv, s->pb) < 0 ||
        !IS_KLV_KEY(klv.key, mxf_random_index_pack_key) ||
        klv.length != length - 20)
        goto end;

    avio_skip(s->pb, klv.length - 12);
    mxf->last_partition = avio_rb64(s->pb);

end:
    avio_seek(s->pb, mxf->run_in, SEEK_SET);
}