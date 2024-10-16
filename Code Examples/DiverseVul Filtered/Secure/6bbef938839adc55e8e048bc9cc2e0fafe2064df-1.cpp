void ff_mpeg4_merge_partitions(MpegEncContext *s)
{
    const int pb2_len    = put_bits_count(&s->pb2);
    const int tex_pb_len = put_bits_count(&s->tex_pb);
    const int bits       = put_bits_count(&s->pb);

    if (s->pict_type == AV_PICTURE_TYPE_I) {
        put_bits(&s->pb, 19, DC_MARKER);
        s->misc_bits  += 19 + pb2_len + bits - s->last_bits;
        s->i_tex_bits += tex_pb_len;
    } else {
        put_bits(&s->pb, 17, MOTION_MARKER);
        s->misc_bits  += 17 + pb2_len;
        s->mv_bits    += bits - s->last_bits;
        s->p_tex_bits += tex_pb_len;
    }

    flush_put_bits(&s->pb2);
    flush_put_bits(&s->tex_pb);

    set_put_bits_buffer_size(&s->pb, s->pb2.buf_end - s->pb.buf);
    avpriv_copy_bits(&s->pb, s->pb2.buf, pb2_len);
    avpriv_copy_bits(&s->pb, s->tex_pb.buf, tex_pb_len);
    s->last_bits = put_bits_count(&s->pb);
}