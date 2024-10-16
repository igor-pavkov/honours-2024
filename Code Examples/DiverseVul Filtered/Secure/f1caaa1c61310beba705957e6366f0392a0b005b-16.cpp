static int dnxhd_write_header(AVCodecContext *avctx, uint8_t *buf)
{
    DNXHDEncContext *ctx = avctx->priv_data;
    static const uint8_t header_prefix[5] = { 0x00,0x00,0x02,0x80,0x01 };

    memset(buf, 0, 640);

    memcpy(buf, header_prefix, 5);
    buf[5] = ctx->interlaced ? ctx->cur_field+2 : 0x01;
    buf[6] = 0x80; // crc flag off
    buf[7] = 0xa0; // reserved
    AV_WB16(buf + 0x18, avctx->height>>ctx->interlaced); // ALPF
    AV_WB16(buf + 0x1a, avctx->width);  // SPL
    AV_WB16(buf + 0x1d, avctx->height>>ctx->interlaced); // NAL

    buf[0x21] = ctx->cid_table->bit_depth == 10 ? 0x58 : 0x38;
    buf[0x22] = 0x88 + (ctx->interlaced<<2);
    AV_WB32(buf + 0x28, ctx->cid); // CID
    buf[0x2c] = ctx->interlaced ? 0 : 0x80;

    buf[0x5f] = 0x01; // UDL

    buf[0x167] = 0x02; // reserved
    AV_WB16(buf + 0x16a, ctx->m.mb_height * 4 + 4); // MSIPS
    buf[0x16d] = ctx->m.mb_height; // Ns
    buf[0x16f] = 0x10; // reserved

    ctx->msip = buf + 0x170;
    return 0;
}