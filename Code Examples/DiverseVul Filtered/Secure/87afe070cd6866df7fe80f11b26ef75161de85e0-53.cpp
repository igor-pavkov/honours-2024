u32 parse_split(char *arg_val, u32 opt)
{
	switch (opt) {
	case 0://-split
		split_duration = atof(arg_val);
		if (split_duration < 0) split_duration = 0;
		split_size = 0;
		break;
	case 1: //-split-rap, -splitr
		split_duration = -1;
		split_size = -1;
		break;
	case 2: //-split-size, -splits
		split_size = (u32)atoi(arg_val);
		split_duration = 0;
		break;
	case 4: //-splitz
		adjust_split_end = 1;
		//fallthrough
	case 3: //-split-chunk, -splitx
		if (!strstr(arg_val, ":")) {
			M4_LOG(GF_LOG_ERROR, ("Chunk extraction usage: \"-splitx start:end\" expressed in seconds\n"));
			return 2;
		}
		if (strstr(arg_val, "end")) {
			if (strstr(arg_val, "end-")) {
				Double dur_end=0;
				sscanf(arg_val, "%lf:end-%lf", &split_start, &dur_end);
				split_duration = -2 - dur_end;
			} else {
				sscanf(arg_val, "%lf:end", &split_start);
				split_duration = -2;
			}
		} else {
			if (strchr(arg_val, '-')) {
				split_range_str = arg_val;
			} else {
				sscanf(arg_val, "%lf:%lf", &split_start, &split_duration);
				split_duration -= split_start;
			}
		}
		split_size = 0;
		break;
	}
	return 0;
}