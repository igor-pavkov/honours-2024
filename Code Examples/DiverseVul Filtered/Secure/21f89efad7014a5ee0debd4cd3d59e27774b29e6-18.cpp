int check_level(const char *level, gnutls_priority_t priority_cache,
		int add)
{
	bulk_rmadd_func *func;
	unsigned profile = 0;
	unsigned i;
	int j;
	const cipher_entry_st *centry;

	if (add)
		func = _add_priority;
	else
		func = _set_priority;

	for (i=0;;i++) {
		if (pgroups[i].name == NULL)
			return 0;

		if (strcasecmp(level, pgroups[i].name) == 0 ||
			(pgroups[i].alias != NULL && strcasecmp(level, pgroups[i].alias) == 0)) {
			if (pgroups[i].proto_list != NULL)
				func(&priority_cache->protocol, *pgroups[i].proto_list);
			func(&priority_cache->cipher, *pgroups[i].cipher_list);
			func(&priority_cache->kx, *pgroups[i].kx_list);
			func(&priority_cache->mac, *pgroups[i].mac_list);
			func(&priority_cache->sign_algo, *pgroups[i].sign_list);
			func(&priority_cache->supported_ecc, *pgroups[i].ecc_list);

			if (pgroups[i].profile != 0) {
				SET_PROFILE(pgroups[i].profile); /* set certificate level */
			}
			SET_LEVEL(pgroups[i].sec_param); /* set DH params level */
			priority_cache->no_tickets = pgroups[i].no_tickets;
			if (priority_cache->have_cbc == 0) {
				for (j=0;(*pgroups[i].cipher_list)[j]!=0;j++) {
					centry = cipher_to_entry((*pgroups[i].cipher_list)[j]);
					if (centry != NULL && centry->type == CIPHER_BLOCK) {
						priority_cache->have_cbc = 1;
						break;
					}
				}
			}
			return 1;
		}
	}
}