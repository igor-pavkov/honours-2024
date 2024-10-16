u32 mp4box_parse_args(int argc, char **argv)
{
	u32 i;
	/*parse our args*/
	for (i = 1; i < (u32)argc; i++) {
		char *arg = argv[i];
		/*input file(s)*/
		if ((arg[0] != '-') || !stricmp(arg, "--")) {
			char *arg_val = arg;
			if (!stricmp(arg, "--")) {
				if (i+1==(u32)argc) {
					M4_LOG(GF_LOG_ERROR, ("Missing arg for `--` - please check usage\n"));
					return 2;
				}
				has_next_arg = GF_TRUE;
				arg_val = argv[i + 1];
				i++;
			}
			if (argc < 3) {
				M4_LOG(GF_LOG_ERROR, ("Error - only one input file found as argument, please check usage\n"));
				return 2;
			}
			else if (inName) {
				if (dash_duration) {
					if (!nb_dash_inputs) {
						dash_inputs = set_dash_input(dash_inputs, inName, &nb_dash_inputs);
					}
					dash_inputs = set_dash_input(dash_inputs, arg_val, &nb_dash_inputs);
				}
				else {
					M4_LOG(GF_LOG_ERROR, ("Error - 2 input names specified, please check usage\n"));
					return 2;
				}
			}
			else {
				inName = arg_val;
			}
		}
		//all deprecated options
		else if (!stricmp(arg, "-grab-ts") || !stricmp(arg, "-atsc") || !stricmp(arg, "-rtp")) {
			M4_LOG(GF_LOG_ERROR, ("Deprecated fuctionnality `%s` - use gpac application\n", arg));
			return 2;
		}
		else if (!stricmp(arg, "-write-buffer")) {
			M4_LOG(GF_LOG_WARNING, ("`%s` option deprecated, use `-bs-cache-size`", arg));
			gf_opts_set_key("temp", "bs-cache-size", argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-pssh-moof")) {
			M4_LOG(GF_LOG_ERROR, ("`-pssh-moof` option deprecated , use `-pssh` option\n"));
			return 2;
		}
		else if (!stricmp(arg, "-tag-list")) {
			M4_LOG(GF_LOG_ERROR, ("`-tag-list` option deprecated, use `-h tags`\n"));
			return 2;
		}
		else if (!stricmp(arg, "-aviraw")) {
			M4_LOG(GF_LOG_ERROR, ("`-aviraw` option deprecated, use `-raw`\n"));
			return 2;
		}
		else if (!stricmp(arg, "-avi")) {
			M4_LOG(GF_LOG_ERROR, ("`-avi` option deprecated, use `-mux`\n"));
			return 2;
		}
		else if (!strncmp(arg, "-p=", 3)) {
			continue;
		}

		//parse argument
		else if (mp4box_parse_single_arg(argc, argv, arg, &i)) {
			if (arg_parse_res)
				return mp4box_cleanup(arg_parse_res);
		}
		//not a MP4Box arg
		else {
			u32 res = gf_sys_is_gpac_arg(arg);
			if (res==0) {
				PrintHelp(arg, GF_FALSE, GF_TRUE);
				return 2;
			} else if (res==2) {
				i++;
			}
		}
		//live scene encoder does not use the unified parsing and should be moved as a scene encoder filter
		if (live_scene) return 0;
	}
	return 0;
}