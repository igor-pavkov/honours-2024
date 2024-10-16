static int adts_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    ADTSContext *adts = s->priv_data;
    AVCodecParameters *par = s->streams[0]->codecpar;
    AVIOContext *pb = s->pb;
    uint8_t buf[ADTS_HEADER_SIZE];

    if (!pkt->size)
        return 0;
    if (!par->extradata_size) {
        uint8_t *side_data;
        size_t side_data_size;
        int ret;

        side_data = av_packet_get_side_data(pkt, AV_PKT_DATA_NEW_EXTRADATA,
                                            &side_data_size);
        if (side_data_size) {
            ret = adts_decode_extradata(s, adts, side_data, side_data_size);
            if (ret < 0)
                return ret;
            ret = ff_alloc_extradata(par, side_data_size);
            if (ret < 0)
                return ret;
            memcpy(par->extradata, side_data, side_data_size);
        }
    }
    if (adts->write_adts) {
        int err = adts_write_frame_header(adts, buf, pkt->size,
                                             adts->pce_size);
        if (err < 0)
            return err;
        avio_write(pb, buf, ADTS_HEADER_SIZE);
        if (adts->pce_size) {
            avio_write(pb, adts->pce_data, adts->pce_size);
            adts->pce_size = 0;
        }
    }
    avio_write(pb, pkt->data, pkt->size);

    return 0;
}