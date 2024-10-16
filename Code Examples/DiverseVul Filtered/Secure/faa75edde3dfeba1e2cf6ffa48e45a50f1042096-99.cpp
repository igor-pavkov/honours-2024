GF_Err gf_laser_decode_command_list(GF_LASeRCodec *codec, u16 ESID, u8 *data, u32 data_len, GF_List *com_list)
{
	GF_Err e;
	u32 i;
	if (!codec || !data || !data_len) return GF_BAD_PARAM;

	codec->info = lsr_get_stream(codec, ESID);
	if (!codec->info) return GF_BAD_PARAM;
	codec->coord_bits = codec->info->cfg.coord_bits;
	codec->scale_bits = codec->info->cfg.scale_bits_minus_coord_bits;
	codec->time_resolution = codec->info->cfg.time_resolution;
	codec->color_scale = (1<<codec->info->cfg.colorComponentBits) - 1;
	if (codec->info->cfg.resolution >= 0)
		codec->res_factor = INT2FIX(1<<codec->info->cfg.resolution);
	else
		codec->res_factor = gf_divfix(FIX_ONE, INT2FIX(1 << (-codec->info->cfg.resolution)) );

	codec->bs = gf_bs_new(data, data_len, GF_BITSTREAM_READ);
	gf_bs_set_eos_callback(codec->bs, lsr_end_of_stream, codec);
	codec->memory_dec = GF_TRUE;
	e = lsr_decode_laser_unit(codec, com_list);
	gf_bs_del(codec->bs);
	codec->bs = NULL;
	if (e) return e;

	for (i=0; i<gf_list_count(codec->unresolved_commands); i++) {
		GF_Command *com = (GF_Command *)gf_list_get(codec->unresolved_commands, i);
		assert(!com->node);
		com->node = gf_sg_find_node(codec->sg, com->RouteID);
		if (com->node) {
			gf_node_register(com->node, NULL);
			com->RouteID = 0;
			gf_list_rem(codec->unresolved_commands, i);
			i--;
		}
	}
	return GF_OK;
}