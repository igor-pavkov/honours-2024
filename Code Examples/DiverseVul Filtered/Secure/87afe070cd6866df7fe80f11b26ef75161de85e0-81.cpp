static u32 do_raw_cat()
{
	char chunk[4096];
	FILE *fin, *fout;
	s64 to_copy, done;
	fin = gf_fopen(raw_cat, "rb");
	if (!fin) return mp4box_cleanup(1);

	fout = gf_fopen(inName, "a+b");
	if (!fout) {
		gf_fclose(fin);
		return mp4box_cleanup(1);
	}
	gf_fseek(fin, 0, SEEK_END);
	to_copy = gf_ftell(fin);
	gf_fseek(fin, 0, SEEK_SET);
	done = 0;
	while (1) {
		u32 nb_bytes = (u32) gf_fread(chunk, 4096, fin);
		gf_fwrite(chunk, nb_bytes, fout);
		done += nb_bytes;
		fprintf(stderr, "Appending file %s - %02.2f done\r", raw_cat, 100.0*done/to_copy);
		if (done >= to_copy) break;
	}
	gf_fclose(fin);
	gf_fclose(fout);
	return mp4box_cleanup(0);
}