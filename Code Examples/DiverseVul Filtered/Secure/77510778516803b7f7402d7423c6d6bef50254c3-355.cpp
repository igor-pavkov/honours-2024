GF_Box *fecr_box_new()
{
	ISOM_DECL_BOX_ALLOC(FECReservoirBox, GF_ISOM_BOX_TYPE_FECR);
	return (GF_Box *)tmp;
}