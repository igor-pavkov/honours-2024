GF_Box *tfdt_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TFBaseMediaDecodeTimeBox, GF_ISOM_BOX_TYPE_TFDT);
	return (GF_Box *)tmp;
}