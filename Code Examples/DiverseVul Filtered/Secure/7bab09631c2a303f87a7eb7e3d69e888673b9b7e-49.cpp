bool xfrm_selector_match(const struct xfrm_selector *sel, const struct flowi *fl,
			 unsigned short family)
{
	switch (family) {
	case AF_INET:
		return __xfrm4_selector_match(sel, fl);
	case AF_INET6:
		return __xfrm6_selector_match(sel, fl);
	}
	return false;
}