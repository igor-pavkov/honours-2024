static unsigned long xfrm_new_hash_mask(unsigned int old_hmask)
{
	return ((old_hmask + 1) << 1) - 1;
}