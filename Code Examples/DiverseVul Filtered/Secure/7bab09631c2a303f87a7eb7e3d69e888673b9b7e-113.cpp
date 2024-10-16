static int migrate_tmpl_match(const struct xfrm_migrate *m, const struct xfrm_tmpl *t)
{
	int match = 0;

	if (t->mode == m->mode && t->id.proto == m->proto &&
	    (m->reqid == 0 || t->reqid == m->reqid)) {
		switch (t->mode) {
		case XFRM_MODE_TUNNEL:
		case XFRM_MODE_BEET:
			if (xfrm_addr_equal(&t->id.daddr, &m->old_daddr,
					    m->old_family) &&
			    xfrm_addr_equal(&t->saddr, &m->old_saddr,
					    m->old_family)) {
				match = 1;
			}
			break;
		case XFRM_MODE_TRANSPORT:
			/* in case of transport mode, template does not store
			   any IP addresses, hence we just compare mode and
			   protocol */
			match = 1;
			break;
		default:
			break;
		}
	}
	return match;
}