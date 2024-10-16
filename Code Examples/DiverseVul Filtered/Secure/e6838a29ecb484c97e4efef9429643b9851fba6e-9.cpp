static bool nfsd_needs_lockd(void)
{
#if defined(CONFIG_NFSD_V3)
	return (nfsd_versions[2] != NULL) || (nfsd_versions[3] != NULL);
#else
	return (nfsd_versions[2] != NULL);
#endif
}