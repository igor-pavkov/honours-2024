static void carve_deps_at_address(RzDyldCache *cache, cache_img_t *img, HtPU *path_to_idx, ut64 address, int *deps, bool printing) {
	ut64 pa = va2pa(address, cache->n_maps, cache->maps, cache->buf, 0, NULL, NULL);
	if (pa == UT64_MAX) {
		return;
	}
	struct MACH0_(mach_header) mh;
	if (rz_buf_fread_at(cache->buf, pa, (ut8 *)&mh, "8i", 1) != sizeof(struct MACH0_(mach_header))) {
		return;
	}
	if (mh.magic != MH_MAGIC_64 || mh.sizeofcmds == 0) {
		return;
	}
	ut64 cmds_at = pa + sizeof(struct MACH0_(mach_header));
	ut8 *cmds = malloc(mh.sizeofcmds + 1);
	if (!cmds || rz_buf_read_at(cache->buf, cmds_at, cmds, mh.sizeofcmds) != mh.sizeofcmds) {
		goto beach;
	}
	cmds[mh.sizeofcmds] = 0;
	ut8 *cursor = cmds;
	ut8 *end = cmds + mh.sizeofcmds;
	while (cursor < end) {
		ut32 cmd = rz_read_le32(cursor);
		ut32 cmdsize = rz_read_le32(cursor + sizeof(ut32));
		if (cmd == LC_LOAD_DYLIB ||
			cmd == LC_LOAD_WEAK_DYLIB ||
			cmd == LC_REEXPORT_DYLIB ||
			cmd == LC_LOAD_UPWARD_DYLIB) {
			bool found;
			if (cursor + 24 >= end) {
				break;
			}
			const char *key = (const char *)cursor + 24;
			size_t dep_index = (size_t)ht_pu_find(path_to_idx, key, &found);
			if (!found || dep_index >= cache->hdr->imagesCount) {
				RZ_LOG_WARN("alien dep '%s'\n", key);
				continue;
			}
			deps[dep_index]++;
			if (printing) {
				RZ_LOG_INFO("-> %s\n", key);
			}
		}
		cursor += cmdsize;
	}

beach:
	free(cmds);
}