static void write_matrix(AVIOContext *pb, int16_t a, int16_t b, int16_t c,
                         int16_t d, int16_t tx, int16_t ty)
{
    avio_wb32(pb, a << 16);  /* 16.16 format */
    avio_wb32(pb, b << 16);  /* 16.16 format */
    avio_wb32(pb, 0);        /* u in 2.30 format */
    avio_wb32(pb, c << 16);  /* 16.16 format */
    avio_wb32(pb, d << 16);  /* 16.16 format */
    avio_wb32(pb, 0);        /* v in 2.30 format */
    avio_wb32(pb, tx << 16); /* 16.16 format */
    avio_wb32(pb, ty << 16); /* 16.16 format */
    avio_wb32(pb, 1 << 30);  /* w in 2.30 format */
}