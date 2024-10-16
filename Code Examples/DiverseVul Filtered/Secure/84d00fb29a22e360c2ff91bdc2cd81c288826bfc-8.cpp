static int jpc_ppxstab_grow(jpc_ppxstab_t *tab, int maxents)
{
	jpc_ppxstabent_t **newents;
	if (tab->maxents < maxents) {
		newents = (tab->ents) ? jas_realloc2(tab->ents, maxents,
		  sizeof(jpc_ppxstabent_t *)) : jas_alloc2(maxents, sizeof(jpc_ppxstabent_t *));
		if (!newents) {
			return -1;
		}
		tab->ents = newents;
		tab->maxents = maxents;
	}
	return 0;
}