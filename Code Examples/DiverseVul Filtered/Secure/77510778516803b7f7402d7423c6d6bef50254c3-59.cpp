GF_Box *fdpa_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_FDpacketBox, GF_ISOM_BOX_TYPE_FDPA);
	return (GF_Box *)tmp;
}