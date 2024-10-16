static void free_bin(RzDyldBinImage *bin) {
	if (!bin) {
		return;
	}
	free(bin->file);
	free(bin);
}