RZ_API void rz_dyldcache_symbols_from_locsym(RzDyldCache *cache, RzDyldBinImage *bin, RzList *symbols, SetU *hash) {
	RzDyldLocSym *locsym = cache->locsym;
	if (!locsym) {
		return;
	}

	if (bin->nlist_start_index >= locsym->nlists_count ||
		bin->nlist_start_index + bin->nlist_count > locsym->nlists_count) {
		RZ_LOG_ERROR("dyldcache: malformed local symbol entry\n");
		return;
	}

	ut64 nlists_size = sizeof(struct MACH0_(nlist)) * bin->nlist_count;
	struct MACH0_(nlist) *nlists = RZ_NEWS0(struct MACH0_(nlist), bin->nlist_count);
	if (!nlists) {
		return;
	}
	ut64 nlists_offset = locsym->local_symbols_offset + locsym->nlists_offset +
		bin->nlist_start_index * sizeof(struct MACH0_(nlist));
	if (rz_buf_fread_at(cache->buf, nlists_offset, (ut8 *)nlists, "iccsl", bin->nlist_count) != nlists_size) {
		free(nlists);
		return;
	}

	ut32 j;
	for (j = 0; j != bin->nlist_count; j++) {
		struct MACH0_(nlist) *nlist = &nlists[j];
		if (set_u_contains(hash, (ut64)nlist->n_value)) {
			continue;
		}
		set_u_add(hash, (ut64)nlist->n_value);
		if (nlist->n_strx >= locsym->strings_size) {
			continue;
		}
		RzBinSymbol *sym = RZ_NEW0(RzBinSymbol);
		if (!sym) {
			break;
		}
		sym->type = "LOCAL";
		sym->vaddr = nlist->n_value;
		ut64 slide = rz_dyldcache_get_slide(cache);
		sym->paddr = va2pa(nlist->n_value, cache->n_maps, cache->maps, cache->buf, slide, NULL, NULL);

		char *symstr = rz_buf_get_string(cache->buf, locsym->local_symbols_offset + locsym->strings_offset + nlist->n_strx);
		if (symstr) {
			sym->name = symstr;
		} else {
			static ut32 k = 0;
			sym->name = rz_str_newf("unk_local%d", k++);
		}

		rz_list_append(symbols, sym);
	}

	free(nlists);
}