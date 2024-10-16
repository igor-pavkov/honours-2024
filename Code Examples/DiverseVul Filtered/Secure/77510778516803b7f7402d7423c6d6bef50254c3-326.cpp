GF_Box *mhap_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MHACompatibleProfilesBox, GF_ISOM_BOX_TYPE_MHAP);
	return (GF_Box *)tmp;
}