GF_Box *SmDm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SMPTE2086MasteringDisplayMetadataBox, GF_ISOM_BOX_TYPE_SMDM);
	return (GF_Box *)tmp;
}