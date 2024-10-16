static inline int selector_cmp(struct xfrm_selector *s1, struct xfrm_selector *s2)
{
	u32 *p1 = (u32 *) s1;
	u32 *p2 = (u32 *) s2;
	int len = sizeof(struct xfrm_selector) / sizeof(u32);
	int i;

	for (i = 0; i < len; i++) {
		if (p1[i] != p2[i])
			return 1;
	}

	return 0;
}