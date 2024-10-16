GF_Err mfra_box_read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read(s, bs, mfra_on_child_box);
}