static Bool PrintHelpArg(char *arg_name, u32 search_type, GF_FilterSession *fs)
{
	Bool first=GF_TRUE;
	GF_GPACArg an_arg;
	u32 i, count;
	u32 res = 0;
	u32 alen = (u32) strlen(arg_name);
	res += PrintHelpForArgs(arg_name, m4b_gen_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_split_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_dash_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_imp_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_imp_fileopt_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_senc_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_crypt_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_hint_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_extr_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_dump_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_meta_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_swf_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_liveenc_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, m4b_usage_args, NULL, search_type);
	res += PrintHelpForArgs(arg_name, NULL, (GF_GPACArg *) gf_sys_get_options(), search_type);

	if (!fs) return res;

	memset(&an_arg, 0, sizeof(GF_GPACArg));
	count = gf_fs_filters_registers_count(fs);
	for (i=0; i<count; i++) {
		u32 j=0;
		const GF_FilterRegister *reg = gf_fs_get_filter_register(fs, i);

		while (reg->args) {
			u32 len;
			const GF_FilterArgs *arg = &reg->args[j];
			if (!arg || !arg->arg_name) break;
			j++;
			if ((search_type==SEARCH_ARG_EXACT) && strcmp(arg->arg_name, arg_name)) continue;

			if ((search_type==SEARCH_ARG_CLOSE) && !gf_sys_word_match(arg->arg_name, arg_name)) continue;

			if (search_type==SEARCH_DESC) {
				if (!strstr_nocase(arg->arg_desc, arg_name, alen)) continue;
			}

			an_arg.name = arg->arg_name;
			if (search_type==SEARCH_ARG_EXACT) {
				an_arg.description = arg->arg_desc;
				switch (arg->arg_type) {
				case GF_PROP_BOOL:
					an_arg.type = GF_ARG_BOOL;
					break;
				case GF_PROP_UINT:
				case GF_PROP_SINT:
					an_arg.type = GF_ARG_INT;
					break;
				case GF_PROP_DOUBLE:
					an_arg.type = GF_ARG_DOUBLE;
					break;
				case GF_PROP_STRING_LIST:
				case GF_PROP_UINT_LIST:
				case GF_PROP_SINT_LIST:
				case GF_PROP_VEC2I_LIST:
					an_arg.type = GF_ARG_STRINGS;
					break;
				case GF_PROP_4CC:
					an_arg.type = GF_ARG_4CC;
					break;
				case GF_PROP_4CC_LIST:
					an_arg.type = GF_ARG_4CCS;
					break;
				default:
					an_arg.type = GF_ARG_STRING;
					break;
				}
				if (first) {
					first = GF_FALSE;
					gf_sys_format_help(helpout, 0, "\nGlobal filter session arguments. Syntax is `--arg` or `--arg=VAL`. `[F]` indicates filter name. See `gpac -h` and `gpac -h F` for more info.\n");
				}
				fprintf(helpout, "[%s]", reg->name);
				len = (u32)strlen(reg->name);
				while (len<10) {
					len++;
					fprintf(helpout, " ");
				}
				fprintf(helpout, " ");
			}

			gf_sys_print_arg(helpout, GF_PRINTARG_ADD_DASH, &an_arg, "TEST");
			res++;
		}
	}
	if (res) return GF_TRUE;
	return GF_FALSE;
}