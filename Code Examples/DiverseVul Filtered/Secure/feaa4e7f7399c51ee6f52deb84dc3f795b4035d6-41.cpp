static RList *sections(RBinFile *bf) {
	RList *ret = NULL;
	RBinObject *obj = bf ? bf->o : NULL;

	if (!obj || !obj->bin_obj || !(ret = r_list_newf ((RListFree)free))) {
		return NULL;
	}

	RKernelCacheObj *kobj = (RKernelCacheObj*) obj->bin_obj;
	ensure_kexts_initialized (kobj, bf);

	int iter;
	RKext *kext;
	r_kext_index_foreach (kobj->kexts, iter, kext) {
		ut8 magicbytes[4];

		r_buf_read_at (kobj->cache_buf, kext->range.offset, magicbytes, 4);
		int magic = r_read_le32 (magicbytes);
		switch (magic) {
		case MH_MAGIC_64:
			sections_from_mach0 (ret, kext->mach0, bf, kext->range.offset, kext->name, kobj);
			break;
		default:
			eprintf ("Unknown sub-bin\n");
			break;
		}
	}

	sections_from_mach0 (ret, kobj->mach0, bf, 0, NULL, kobj);

	struct MACH0_(segment_command) *seg;
	int nsegs = R_MIN (kobj->mach0->nsegs, 128);
	int i;
	for (i = 0; i < nsegs; i++) {
		RBinSection *ptr;
		char segname[17];

		if (!(ptr = R_NEW0 (RBinSection))) {
			break;
		}

		seg = &kobj->mach0->segs[i];
		r_str_ncpy (segname, seg->segname, 17);
		r_str_filter (segname, -1);
		ptr->name = r_str_newf ("%d.%s", i, segname);
		ptr->size = seg->vmsize;
		ptr->vsize = seg->vmsize;
		ptr->paddr = seg->fileoff + bf->o->boffset;
		ptr->vaddr = seg->vmaddr;
		ptr->add = true;
		ptr->is_segment = true;
		if (!ptr->vaddr) {
			ptr->vaddr = ptr->paddr;
		}
		ptr->perm = prot2perm (seg->initprot);
		r_list_append (ret, ptr);
	}

	return ret;
}