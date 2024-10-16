GF_Box *sinf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ProtectionSchemeInfoBox, GF_ISOM_BOX_TYPE_SINF);
	return (GF_Box *)tmp;
}