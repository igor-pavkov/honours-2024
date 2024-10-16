GF_Box *piff_pssh_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_PIFFProtectionSystemHeaderBox, GF_ISOM_BOX_TYPE_UUID);
	tmp->internal_4cc = GF_ISOM_BOX_UUID_PSSH;
	return (GF_Box *)tmp;
}