GF_Box *sbgp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleGroupBox, GF_ISOM_BOX_TYPE_SBGP);
	return (GF_Box *)tmp;
}