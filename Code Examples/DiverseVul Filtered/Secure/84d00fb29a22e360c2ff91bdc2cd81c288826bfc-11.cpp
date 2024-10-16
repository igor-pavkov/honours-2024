static jpc_ppxstabent_t *jpc_ppxstabent_create()
{
	jpc_ppxstabent_t *ent;
	if (!(ent = jas_malloc(sizeof(jpc_ppxstabent_t)))) {
		return 0;
	}
	ent->data = 0;
	ent->len = 0;
	ent->ind = 0;
	return ent;
}