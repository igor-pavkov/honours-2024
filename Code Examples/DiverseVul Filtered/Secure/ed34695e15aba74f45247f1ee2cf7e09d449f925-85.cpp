static bool ff_mirror_match_fh(const struct nfs4_ff_layout_mirror *m1,
		const struct nfs4_ff_layout_mirror *m2)
{
	int i, j;

	if (m1->fh_versions_cnt != m2->fh_versions_cnt)
		return false;
	for (i = 0; i < m1->fh_versions_cnt; i++) {
		bool found_fh = false;
		for (j = 0; j < m2->fh_versions_cnt; j++) {
			if (nfs_compare_fh(&m1->fh_versions[i],
					&m2->fh_versions[j]) == 0) {
				found_fh = true;
				break;
			}
		}
		if (!found_fh)
			return false;
	}
	return true;
}