static void nfs_unset_verifier_delegated(unsigned long *verf)
{
	*verf &= ~1UL;
}