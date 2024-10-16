R_API RList *r_bin_file_set_hashes(RBin *bin, RList/*<RBinFileHash*/ *new_hashes) {
	r_return_val_if_fail (bin && bin->cur && bin->cur->o && bin->cur->o->info, NULL);
	RBinFile *bf = bin->cur;
	RBinInfo *info = bf->o->info;

	RList *prev_hashes = info->file_hashes;
	info->file_hashes = new_hashes;

	return prev_hashes;
}