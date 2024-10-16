static bool nfs_test_verifier_delegated(unsigned long verf)
{
	return verf & 1;
}