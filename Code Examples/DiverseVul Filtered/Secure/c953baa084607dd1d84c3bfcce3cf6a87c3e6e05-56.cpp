static int rb_size(AVIOContext *pb, uint64_t* value, int size)
{
    if (size == 0)
        *value = 0;
    else if (size == 1)
        *value = avio_r8(pb);
    else if (size == 2)
        *value = avio_rb16(pb);
    else if (size == 4)
        *value = avio_rb32(pb);
    else if (size == 8)
        *value = avio_rb64(pb);
    else
        return -1;
    return size;
}