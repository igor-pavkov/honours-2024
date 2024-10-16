int cipso_v4_getattr(const unsigned char *cipso,
		     struct netlbl_lsm_secattr *secattr)
{
	int ret_val = -ENOMSG;
	u32 doi;
	struct cipso_v4_doi *doi_def;

	if (cipso_v4_cache_check(cipso, cipso[1], secattr) == 0)
		return 0;

	doi = get_unaligned_be32(&cipso[2]);
	rcu_read_lock();
	doi_def = cipso_v4_doi_search(doi);
	if (!doi_def)
		goto getattr_return;
	/* XXX - This code assumes only one tag per CIPSO option which isn't
	 * really a good assumption to make but since we only support the MAC
	 * tags right now it is a safe assumption. */
	switch (cipso[6]) {
	case CIPSO_V4_TAG_RBITMAP:
		ret_val = cipso_v4_parsetag_rbm(doi_def, &cipso[6], secattr);
		break;
	case CIPSO_V4_TAG_ENUM:
		ret_val = cipso_v4_parsetag_enum(doi_def, &cipso[6], secattr);
		break;
	case CIPSO_V4_TAG_RANGE:
		ret_val = cipso_v4_parsetag_rng(doi_def, &cipso[6], secattr);
		break;
	case CIPSO_V4_TAG_LOCAL:
		ret_val = cipso_v4_parsetag_loc(doi_def, &cipso[6], secattr);
		break;
	}
	if (ret_val == 0)
		secattr->type = NETLBL_NLTYPE_CIPSOV4;

getattr_return:
	rcu_read_unlock();
	return ret_val;
}