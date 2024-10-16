GF_Err gf_laser_decoder_configure_stream(GF_LASeRCodec *codec, u16 ESID, u8 *dsi, u32 dsi_len)
{
	LASeRStreamInfo *info;
	GF_BitStream *bs;
	if (lsr_get_stream(codec, ESID) != NULL) return GF_BAD_PARAM;
	GF_SAFEALLOC(info, LASeRStreamInfo);
	if (!info) return GF_OUT_OF_MEM;
	info->ESID = ESID;
	bs = gf_bs_new(dsi, dsi_len, GF_BITSTREAM_READ);

	info->cfg.profile = gf_bs_read_int(bs, 8);
	info->cfg.level = gf_bs_read_int(bs, 8);
	/*info->cfg.reserved = */ gf_bs_read_int(bs, 3);
	info->cfg.pointsCodec = gf_bs_read_int(bs, 2);
	info->cfg.pathComponents = gf_bs_read_int(bs, 4);
	info->cfg.fullRequestHost = gf_bs_read_int(bs, 1);
	if (gf_bs_read_int(bs, 1)) {
		info->cfg.time_resolution = gf_bs_read_int(bs, 16);
	} else {
		info->cfg.time_resolution = 1000;
	}
	info->cfg.colorComponentBits = gf_bs_read_int(bs, 4);
	info->cfg.colorComponentBits += 1;
	info->cfg.resolution = gf_bs_read_int(bs, 4);
	if (info->cfg.resolution>7) info->cfg.resolution -= 16;
	info->cfg.coord_bits = gf_bs_read_int(bs, 5);
	info->cfg.scale_bits_minus_coord_bits = gf_bs_read_int(bs, 4);
	info->cfg.newSceneIndicator = gf_bs_read_int(bs, 1);
	/*reserved*/ gf_bs_read_int(bs, 3);
	info->cfg.extensionIDBits = gf_bs_read_int(bs, 4);
	/*we ignore the rest*/
	gf_list_add(codec->streamInfo, info);
	gf_bs_del(bs);
	return GF_OK;
}