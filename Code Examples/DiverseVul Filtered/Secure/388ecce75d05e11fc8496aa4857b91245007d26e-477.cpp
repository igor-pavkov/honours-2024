GF_Box *tkhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackHeaderBox, GF_ISOM_BOX_TYPE_TKHD);
	tmp->matrix[0] = 0x00010000;
	tmp->matrix[4] = 0x00010000;
	tmp->matrix[8] = 0x40000000;
	return (GF_Box *)tmp;
}