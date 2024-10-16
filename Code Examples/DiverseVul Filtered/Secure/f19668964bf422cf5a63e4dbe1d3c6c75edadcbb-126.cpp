GF_Box *pssh_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ProtectionSystemHeaderBox, GF_ISOM_BOX_TYPE_PSSH);
	return (GF_Box *)tmp;
}