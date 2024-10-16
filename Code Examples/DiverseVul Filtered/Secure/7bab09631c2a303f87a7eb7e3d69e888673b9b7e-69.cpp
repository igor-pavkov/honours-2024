static inline int secpath_has_nontransport(const struct sec_path *sp, int k, int *idxp)
{
	for (; k < sp->len; k++) {
		if (sp->xvec[k]->props.mode != XFRM_MODE_TRANSPORT) {
			*idxp = k;
			return 1;
		}
	}

	return 0;
}