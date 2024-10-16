void ff_mpeg4_encode_video_packet_header(MpegEncContext *s)
{
    int mb_num_bits = av_log2(s->mb_num - 1) + 1;

    put_bits(&s->pb, ff_mpeg4_get_video_packet_prefix_length(s), 0);
    put_bits(&s->pb, 1, 1);

    put_bits(&s->pb, mb_num_bits, s->mb_x + s->mb_y * s->mb_width);
    put_bits(&s->pb, s->quant_precision, s->qscale);
    put_bits(&s->pb, 1, 0); /* no HEC */
}