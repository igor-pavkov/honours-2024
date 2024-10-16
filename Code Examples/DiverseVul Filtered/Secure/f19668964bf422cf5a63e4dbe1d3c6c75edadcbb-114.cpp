GF_Err odtt_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_OMADRMTransactionTrackingBox *ptr = (GF_OMADRMTransactionTrackingBox *)s;

	gf_bs_read_data(bs, ptr->TransactionID, 16);
	ISOM_DECREASE_SIZE(ptr, 16);
	return GF_OK;
}