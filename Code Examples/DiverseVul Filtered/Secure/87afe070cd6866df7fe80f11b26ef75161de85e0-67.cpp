static GF_Err hash_file(char *name, u32 dump_std)
{
	u32 i;
	u8 hash[20];
	GF_Err e = gf_media_get_file_hash(name, hash);
	if (e) return e;
	if (dump_std==2) {
		gf_fwrite(hash, 20, stdout);
	} else if (dump_std==1) {
		for (i=0; i<20; i++) fprintf(stdout, "%02X", hash[i]);
	}
	fprintf(stderr, "File hash (SHA-1): ");
	for (i=0; i<20; i++) fprintf(stderr, "%02X", hash[i]);
	fprintf(stderr, "\n");

	return GF_OK;
}