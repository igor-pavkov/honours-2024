GF_Box *sdp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SDPBox, GF_ISOM_BOX_TYPE_SDP);
	return (GF_Box *)tmp;
}