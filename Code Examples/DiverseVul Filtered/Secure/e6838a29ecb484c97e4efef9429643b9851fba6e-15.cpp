int nfsd_vers(int vers, enum vers_op change)
{
	if (vers < NFSD_MINVERS || vers >= NFSD_NRVERS)
		return 0;
	switch(change) {
	case NFSD_SET:
		nfsd_versions[vers] = nfsd_version[vers];
#if defined(CONFIG_NFSD_V2_ACL) || defined(CONFIG_NFSD_V3_ACL)
		if (vers < NFSD_ACL_NRVERS)
			nfsd_acl_versions[vers] = nfsd_acl_version[vers];
#endif
		break;
	case NFSD_CLEAR:
		nfsd_versions[vers] = NULL;
#if defined(CONFIG_NFSD_V2_ACL) || defined(CONFIG_NFSD_V3_ACL)
		if (vers < NFSD_ACL_NRVERS)
			nfsd_acl_versions[vers] = NULL;
#endif
		break;
	case NFSD_TEST:
		return nfsd_versions[vers] != NULL;
	case NFSD_AVAIL:
		return nfsd_version[vers] != NULL;
	}
	return 0;
}