static RBinString *__stringAt(RBinFile *bf, RList *ret, ut64 addr) {
	if (addr != 0 && addr != UT64_MAX) {
		return ht_up_find (bf->o->strings_db, addr, NULL);
	}
	return NULL;
}