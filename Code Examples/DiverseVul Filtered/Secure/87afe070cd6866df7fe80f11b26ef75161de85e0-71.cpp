u32 parse_store_mode(char *arg_val, u32 opt)
{
	do_save = GF_TRUE;
	if ((opt == 0) || (opt == 1)) {
		interleaving_time = atof(arg_val) / 1000;
		if (!interleaving_time) do_flat = 2;
		open_edit = GF_TRUE;
		no_inplace = GF_TRUE;
		if (opt==1) old_interleave = 1;
	} else if (opt==2) {
		interleaving_time = atof(arg_val);
		do_frag = GF_TRUE;
	} else {
		force_new = 2;
		interleaving_time = 0.5;
		do_flat = 1;
	}
	return 0;
}