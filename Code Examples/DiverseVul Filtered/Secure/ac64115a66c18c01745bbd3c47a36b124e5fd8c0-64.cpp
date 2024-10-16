void kvmppc_free_lpid(long lpid)
{
	clear_bit(lpid, lpid_inuse);
}