GF_Box *odtt_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_OMADRMTransactionTrackingBox, GF_ISOM_BOX_TYPE_ODTT);
	return (GF_Box *)tmp;
}