void ff_mpeg4_init_partitions(MpegEncContext *s)
{
    uint8_t *start = put_bits_ptr(&s->pb);
    uint8_t *end   = s->pb.buf_end;
    int size       = end - start;
    int pb_size    = (((intptr_t)start + size / 3) & (~3)) - (intptr_t)start;
    int tex_size   = (size - 2 * pb_size) & (~3);

    set_put_bits_buffer_size(&s->pb, pb_size);
    init_put_bits(&s->tex_pb, start + pb_size, tex_size);
    init_put_bits(&s->pb2, start + pb_size + tex_size, pb_size);
}