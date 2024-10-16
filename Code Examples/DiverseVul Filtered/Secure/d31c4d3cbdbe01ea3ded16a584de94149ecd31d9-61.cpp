R_API RList* r_bin_raw_strings(RBinFile *a, int min) {
	RList *l = NULL;
	if (a) {
		int tmp = a->rawstr;
		a->rawstr = 2;
		l = get_strings (a, min, 0);
		a->rawstr = tmp;
	}
	return l;
}