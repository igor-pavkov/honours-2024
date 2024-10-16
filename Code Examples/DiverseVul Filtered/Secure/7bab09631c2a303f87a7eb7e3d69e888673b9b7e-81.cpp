static bool xfrm_migrate_selector_match(const struct xfrm_selector *sel_cmp,
					const struct xfrm_selector *sel_tgt)
{
	if (sel_cmp->proto == IPSEC_ULPROTO_ANY) {
		if (sel_tgt->family == sel_cmp->family &&
		    xfrm_addr_equal(&sel_tgt->daddr, &sel_cmp->daddr,
				    sel_cmp->family) &&
		    xfrm_addr_equal(&sel_tgt->saddr, &sel_cmp->saddr,
				    sel_cmp->family) &&
		    sel_tgt->prefixlen_d == sel_cmp->prefixlen_d &&
		    sel_tgt->prefixlen_s == sel_cmp->prefixlen_s) {
			return true;
		}
	} else {
		if (memcmp(sel_tgt, sel_cmp, sizeof(*sel_tgt)) == 0) {
			return true;
		}
	}
	return false;
}