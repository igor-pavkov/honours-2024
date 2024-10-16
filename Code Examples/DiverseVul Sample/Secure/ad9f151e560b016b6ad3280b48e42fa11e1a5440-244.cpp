void nf_tables_trans_destroy_flush_work(void)
{
	flush_work(&trans_destroy_work);
}