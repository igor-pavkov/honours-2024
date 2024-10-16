void lsr_exec_command_list(GF_Node *node, void *par, Bool is_destroy)
{
	GF_DOMUpdates *up = (GF_DOMUpdates *)node;
	GF_LASeRCodec *codec = (GF_LASeRCodec *)gf_node_get_private((GF_Node*)node);

	if (is_destroy || !up || (up->sgprivate->tag!=TAG_DOMUpdates)) return;
	assert(!codec->bs);

	codec->info = lsr_get_stream(codec, 0);
	if (!codec->info) return;
	codec->coord_bits = codec->info->cfg.coord_bits;
	codec->scale_bits = codec->info->cfg.scale_bits_minus_coord_bits;
	codec->time_resolution = codec->info->cfg.time_resolution;
	codec->color_scale = (1<<codec->info->cfg.colorComponentBits) - 1;
	if (codec->info->cfg.resolution >= 0)
		codec->res_factor = INT2FIX(1<<codec->info->cfg.resolution);
	else
		codec->res_factor = gf_divfix(FIX_ONE, INT2FIX(1 << (-codec->info->cfg.resolution)) );

	codec->bs = gf_bs_new(up->data, up->data_size, GF_BITSTREAM_READ);
	codec->memory_dec = 0;
	lsr_read_command_list(codec, NULL, NULL, 0);
	gf_bs_del(codec->bs);
	codec->bs = NULL;
}