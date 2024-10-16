GF_Err edts_box_read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read(s, bs, edts_on_child_box);
}