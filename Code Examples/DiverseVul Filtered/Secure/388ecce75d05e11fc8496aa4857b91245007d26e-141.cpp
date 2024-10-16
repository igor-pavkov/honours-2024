GF_Box *ftyp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_FileTypeBox, GF_ISOM_BOX_TYPE_FTYP);
	return (GF_Box *)tmp;
}