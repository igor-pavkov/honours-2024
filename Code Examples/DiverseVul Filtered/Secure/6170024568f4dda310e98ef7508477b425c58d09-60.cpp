GF_Err wide_box_read(GF_Box *s, GF_BitStream *bs)
{
	gf_bs_skip_bytes(bs, s->size);
	s->size = 0;
	return GF_OK;
}