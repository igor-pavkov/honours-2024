R_API int r_egg_run(REgg *egg) {
	ut64 tmpsz;
	const ut8 *tmp = r_buf_data (egg->bin, &tmpsz);
	bool res = r_sys_run (tmp, tmpsz);
	return res;
}