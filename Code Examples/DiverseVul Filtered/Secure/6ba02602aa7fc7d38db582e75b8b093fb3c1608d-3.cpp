static av_always_inline int vmnc_get_pixel(GetByteContext *gb, int bpp, int be)
{
    switch (bpp * 2 + be) {
    case 2:
    case 3:
        return bytestream2_get_byte(gb);
    case 4:
        return bytestream2_get_le16(gb);
    case 5:
        return bytestream2_get_be16(gb);
    case 8:
        return bytestream2_get_le32(gb);
    case 9:
        return bytestream2_get_be32(gb);
    default: return 0;
    }
}