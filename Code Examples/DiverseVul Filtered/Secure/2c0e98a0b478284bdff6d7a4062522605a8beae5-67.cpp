static void put_descr(AVIOContext *pb, int tag, unsigned int size)
{
    int i = 3;
    avio_w8(pb, tag);
    for (; i > 0; i--)
        avio_w8(pb, (size >> (7 * i)) | 0x80);
    avio_w8(pb, size & 0x7F);
}