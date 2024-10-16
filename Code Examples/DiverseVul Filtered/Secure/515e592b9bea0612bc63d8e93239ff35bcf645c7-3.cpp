static RBinInfo *info(RBinFile *bf) {
	SymbolsMetadata sm = parseMetadata (bf->buf, 0x40);
	RBinInfo *ret = R_NEW0 (RBinInfo);
	if (!ret) {
		return NULL;
	}
	ret->file = strdup (bf->file);
	ret->bclass = strdup ("symbols");
	ret->os = strdup ("unknown");
	ret->arch = sm.arch ? strdup (sm.arch) : NULL;
	ret->bits = sm.bits;
	ret->type = strdup ("Symbols file");
	ret->subsystem = strdup ("llvm");
	ret->has_va = true;

	return ret;
}