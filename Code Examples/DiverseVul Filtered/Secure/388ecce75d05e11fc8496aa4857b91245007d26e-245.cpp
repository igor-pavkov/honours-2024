GF_Box *tfxd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MSSTimeExtBox, GF_ISOM_BOX_TYPE_UUID);
	tmp->internal_4cc = GF_ISOM_BOX_UUID_TFXD;
	return (GF_Box *)tmp;
}