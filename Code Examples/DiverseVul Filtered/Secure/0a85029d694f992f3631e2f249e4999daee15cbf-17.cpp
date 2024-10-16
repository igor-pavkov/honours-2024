GF_Box *gf_isom_clone_config_box(GF_Box *box)
{
	u8 *data=NULL;
	u32 size=0;
	GF_Err e;
	GF_Box *clone=NULL;
	GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	e = gf_isom_box_size(box);
	if (!e)
		e = gf_isom_box_write(box, bs);

	gf_bs_get_content(bs, &data, &size);
	gf_bs_del(bs);
	if (!e) {
		bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
		e = gf_isom_box_parse(&clone, bs);
		gf_bs_del(bs);
	}
	if (data)
		gf_free(data);
	if (e) {
		if (clone)
			gf_isom_box_del(clone);
		clone = NULL;
	}
	return clone;
}