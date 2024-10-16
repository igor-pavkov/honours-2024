void odtt_box_del(GF_Box *s)
{
	GF_OMADRMTransactionTrackingBox *ptr = (GF_OMADRMTransactionTrackingBox*)s;
	gf_free(ptr);
}