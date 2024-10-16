static char *get_lib_name(RzBuffer *cache_buf, cache_img_t *img) {
	char file[256];
	char *lib_name = file;
	if (rz_buf_read_at(cache_buf, img->pathFileOffset, (ut8 *)&file, sizeof(file)) == sizeof(file)) {
		file[255] = 0;
		return strdup(lib_name);
	}
	return strdup("FAIL");
}