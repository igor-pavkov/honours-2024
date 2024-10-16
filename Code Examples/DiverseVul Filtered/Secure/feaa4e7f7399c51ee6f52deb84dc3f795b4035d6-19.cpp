static void r_kext_free(RKext *kext) {
	if (!kext) {
		return;
	}

	if (kext->mach0) {
		MACH0_(mach0_free) (kext->mach0);
		kext->mach0 = NULL;
	}

	if (kext->own_name && kext->name) {
		R_FREE (kext->name);
		kext->name = NULL;
	}

	R_FREE (kext);
}