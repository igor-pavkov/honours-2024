GF_Box *stvi_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_StereoVideoBox, GF_ISOM_BOX_TYPE_STVI);
	return (GF_Box *)tmp;
}