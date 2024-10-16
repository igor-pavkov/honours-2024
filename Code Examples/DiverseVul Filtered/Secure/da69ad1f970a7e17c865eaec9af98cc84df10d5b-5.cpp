static Bool flac_parse_header(GF_FLACDmxCtx *ctx, char *data, u32 size, FLACHeader *hdr)
{
	u32 block_size, sample_rate, res, top, pos, crc, crc_hdr;

	gf_bs_reassign_buffer(ctx->bs, data, size);
	gf_bs_read_int(ctx->bs, 15);
	/*block_strategy = */gf_bs_read_int(ctx->bs, 1);
	block_size = gf_bs_read_int(ctx->bs, 4);
	sample_rate = gf_bs_read_int(ctx->bs, 4);
	/*u32 channel_layout = */gf_bs_read_int(ctx->bs, 4);
	/*u32 bps = */gf_bs_read_int(ctx->bs, 3);
	gf_bs_read_int(ctx->bs, 1);

	res = gf_bs_read_u8(ctx->bs);
	top = (res & 128) >> 1;
	if ((res & 0xC0) == 0x80 || (res >= 0xFE)) return GF_FALSE;
	while (res & top) {
		s32 tmp = gf_bs_read_u8(ctx->bs);
		tmp -= 128;
		if(tmp>>6)
			return GF_FALSE;
		res = (res<<6) + tmp;
		top <<= 5;
	}
	//res &= (top << 1) - 1;

	if (block_size==6) block_size = 1 + gf_bs_read_int(ctx->bs, 8);
	else if (block_size==7) block_size = 1 + gf_bs_read_int(ctx->bs, 16);
	else {
		block_size = flac_dmx_block_sizes[block_size];
	}

#if 0
	if (bps==0) bps = ctx->bits_per_sample;
	else if (bps==1) bps = 8;
	else if (bps==2) bps = 12;
	else if (bps==4) bps = 16;
	else if (bps==5) bps = 20;
	else if (bps==6) bps = 24;
#endif

	if (sample_rate==0) sample_rate = ctx->sample_rate;
	else if ((sample_rate&0xC)==0xC) {
		if (sample_rate==0xC) sample_rate = gf_bs_read_u8(ctx->bs);
		else if (sample_rate==0xD) sample_rate = gf_bs_read_u16(ctx->bs);
		else if (sample_rate==0xE) sample_rate = 10*gf_bs_read_u16(ctx->bs);
	} else {
		sample_rate = flac_dmx_samplerates[sample_rate];
	}

	pos = (u32) gf_bs_get_position(ctx->bs);

	crc = gf_bs_read_u8(ctx->bs);
	crc_hdr = flac_dmx_crc8(data, pos);

	if (crc != crc_hdr) {
		return GF_FALSE;
	}
	hdr->sample_rate = sample_rate;
	hdr->block_size = block_size;
	return GF_TRUE;
}