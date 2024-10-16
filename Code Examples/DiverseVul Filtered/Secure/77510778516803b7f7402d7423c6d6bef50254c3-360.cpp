GF_Err prft_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_ProducerReferenceTimeBox *ptr = (GF_ProducerReferenceTimeBox *) s;

	ISOM_DECREASE_SIZE(ptr, 12);
	ptr->refTrackID = gf_bs_read_u32(bs);
	ptr->ntp = gf_bs_read_u64(bs);
	if (ptr->version==0) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->timestamp = gf_bs_read_u32(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->timestamp = gf_bs_read_u64(bs);
	}
	return GF_OK;
}