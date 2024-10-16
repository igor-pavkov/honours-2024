static RList *filter_kexts(RKernelCacheObj *obj, RBinFile *bf) {
	RCFValueArray *kext_array = NULL;
	RListIter *iter;
	RCFKeyValue *item;
	r_list_foreach (obj->prelink_info->pairs, iter, item) {
		if (!strcmp (item->key, "_PrelinkInfoDictionary")) {
			kext_array = (RCFValueArray*) item->value;
			break;
		}
	}

	if (!kext_array) {
		return NULL;
	}

	RList *kexts = r_list_newf ((RListFree) &r_kext_free);
	if (!kexts) {
		return NULL;
	}

	bool is_sorted = true;
	RKext *prev_kext = NULL;
	RCFValueDict *kext_item;
	r_list_foreach (kext_array->values, iter, kext_item) {
		RKext *kext = R_NEW0 (RKext);
		if (!kext) {
			R_FREE (kexts);
			return NULL;
		}

		int kext_incomplete = 5;
		RListIter *internal_iter;
		r_list_foreach (kext_item->pairs, internal_iter, item) {
			if (!strcmp (item->key, "CFBundlePackageType")) {
				if (item->value->type != R_CF_STRING) {
					break;
				}
				RCFValueString *type = (RCFValueString*) item->value;
				if (strcmp (type->value, "KEXT")) {
					break;
				}
				kext_incomplete--;
			}

			if (!strcmp (item->key, "_PrelinkExecutableLoadAddr")) {
				if (item->value->type == R_CF_INTEGER) {
					kext_incomplete--;
					kext->vaddr = ((RCFValueInteger*) item->value)->value;
					kext->range.offset = kext->vaddr - obj->pa2va_exec;
				}
			}

			if (!strcmp (item->key, "_PrelinkExecutableSize")) {
				kext_incomplete--;
				if (item->value->type == R_CF_INTEGER) {
					kext->range.size = ((RCFValueInteger*) item->value)->value;
				} else {
					kext->range.size = 0;
				}
			}

			if (!strcmp (item->key, "_PrelinkKmodInfo")) {
				if (item->value->type == R_CF_INTEGER) {
					kext_incomplete--;
					kext->mod_info = ((RCFValueInteger*) item->value)->value;
					kext->mod_info -= obj->pa2va_data;
				}
			}

			if (!strcmp (item->key, "CFBundleIdentifier")) {
				if (item->value->type == R_CF_STRING) {
					kext_incomplete--;
					kext->name = ((RCFValueString*) item->value)->value;
				}
			}
		}

		if (kext_incomplete) {
			r_kext_free (kext);
			continue;
		}

		if (prev_kext && kext->vaddr < prev_kext->vaddr) {
			is_sorted = false;
		}
		prev_kext = kext;

		kext->mach0 = create_kext_mach0 (obj, kext, bf);
		if (!kext->mach0) {
			r_kext_free (kext);
			continue;
		}

		r_kext_fill_text_range (kext);

		r_list_push (kexts, kext);
	}

	if (!is_sorted) {
		eprintf ("SORTING KEXTs...\n");
		r_list_sort (kexts, kexts_sort_vaddr_func);
	}
	return kexts;
}