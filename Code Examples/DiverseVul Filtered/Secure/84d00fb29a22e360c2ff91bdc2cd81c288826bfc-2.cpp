static jpc_ppxstab_t *jpc_ppxstab_create()
{
	jpc_ppxstab_t *tab;

	if (!(tab = jas_malloc(sizeof(jpc_ppxstab_t)))) {
		return 0;
	}
	tab->numents = 0;
	tab->maxents = 0;
	tab->ents = 0;
	return tab;
}