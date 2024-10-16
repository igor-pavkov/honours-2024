GF_Box *smhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SoundMediaHeaderBox, GF_ISOM_BOX_TYPE_SMHD);
	return (GF_Box *)tmp;
}