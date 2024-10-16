static void nfs_set_verifier_delegated(unsigned long *verf)
{
	*verf |= 1UL;
}