u32 mp4box_parse_single_arg_class(int argc, char **argv, char *arg, u32 *arg_index, MP4BoxArg *arg_class)
{
	MP4BoxArg *arg_desc = NULL;
	char *arg_val = NULL;
	u32 i=0;
	while (arg_class[i].name) {
		arg_desc = (MP4BoxArg *) &arg_class[i];
		i++;

#ifdef TEST_ARGS
		char *sep = strchr(arg_desc->name, ' ');
		if (sep) {
			M4_LOG(GF_LOG_ERROR, ("Invalid arg %s, space not allowed\n", arg_desc->name));
			exit(1);
		}
#endif
		if (!strcmp(arg_desc->name, arg+1))
			break;
		if (arg_desc->altname && !strcmp(arg_desc->altname, arg+1))
			break;

		if (arg_desc->parse_flags & ARG_IS_FUN2) {
			if (!strncmp(arg_desc->name, arg+1, strlen(arg_desc->name) ))
				break;
		}
		arg_desc = NULL;
	}
	if (!arg_desc)
		return GF_FALSE;

	if (arg_desc->parse_flags & ARG_OPEN_EDIT) open_edit = GF_TRUE;
	if (arg_desc->parse_flags & ARG_NEED_SAVE) do_save = GF_TRUE;
	if (arg_desc->parse_flags & ARG_NO_INPLACE) no_inplace = GF_TRUE;

	if (arg_desc->type != GF_ARG_BOOL) {
		Bool has_next = GF_TRUE;
		if (*arg_index + 1 == (u32) argc)
			has_next = GF_FALSE;
		else if (argv[*arg_index + 1][0] == '-') {
			s32 v;
			if (sscanf(argv[*arg_index + 1], "%d", &v)!=1)
				has_next = GF_FALSE;
		}
		if (!has_next && ! (arg_desc->parse_flags & ARG_EMPTY) ) {
			M4_LOG(GF_LOG_ERROR, ("Missing argument value for %s - please check usage\n", arg));
			arg_parse_res = 2;
			return GF_TRUE;
		}

		if (has_next && (arg_desc->parse_flags & ARG_EMPTY) && (arg_desc->type==GF_ARG_INT)) {
			s32 ival;
			if (sscanf(argv[*arg_index + 1], "%d", &ival) != 1) {
				has_next = GF_FALSE;
				arg_val = NULL;
			}
		}
		if (has_next) {
			has_next_arg = GF_TRUE;
			*arg_index += 1;
			arg_val = argv[*arg_index];
		}
	}
	if (!arg_desc->arg_ptr) return GF_TRUE;

	if (arg_desc->parse_flags & (ARG_IS_FUN|ARG_IS_FUN2) ) {
		u32 res;
		if (arg_desc->parse_flags & ARG_PUSH_SYSARGS)
			gf_sys_set_args(argc, (const char**) argv);

		if (arg_desc->parse_flags & ARG_IS_FUN) {
			parse_arg_fun fun = (parse_arg_fun) arg_desc->arg_ptr;
			res = fun(arg_val, arg_desc->argv_val);
		} else {
			parse_arg_fun2 fun2 = (parse_arg_fun2) arg_desc->arg_ptr;
			res = fun2(arg, arg_val, arg_desc->argv_val);
		}
		//rewind, not our arg
		if ((res==3) && argv) {
			*arg_index -= 1;
			res = 0;
		}
		arg_parse_res = res;
		return GF_TRUE;
	}

	if (arg_desc->parse_flags & ARG_INT_INC) {
		* (u32 *) arg_desc->arg_ptr += 1;
		return GF_TRUE;
	}

	if (arg_desc->type == GF_ARG_BOOL) {
		if (!arg_desc->parse_flags) {
			if (arg_desc->argv_val) {
				* (u32 *) arg_desc->arg_ptr = arg_desc->argv_val;
			} else {
				* (Bool *) arg_desc->arg_ptr = GF_TRUE;
			}
		} else if (arg_desc->parse_flags & ARG_BOOL_REV) {
			* (Bool *) arg_desc->arg_ptr = GF_FALSE;
		} else if (arg_desc->parse_flags & ARG_HAS_VALUE) {
			* (u32 *) arg_desc->arg_ptr = 0;
		} else if (arg_desc->parse_flags & ARG_BIT_MASK) {
			* (u32 *) arg_desc->arg_ptr |= arg_desc->argv_val;
		} else if (arg_desc->parse_flags & ARG_BIT_MASK_REM) {
			* (u32 *) arg_desc->arg_ptr &= ~arg_desc->argv_val;
		} else if (arg_desc->argv_val) {
			* (u32 *) arg_desc->arg_ptr = arg_desc->argv_val;
		} else {
			* (u32 *) arg_desc->arg_ptr = GF_TRUE;
		}
		return GF_TRUE;
	}

	if (arg_desc->type == GF_ARG_STRING) {
		if (arg_desc->parse_flags & ARG_IS_4CC) {
			u32 alen = arg_val ? (u32) strlen(arg_val) : 0;
			if ((alen<3) || (alen>4)) {
				M4_LOG(GF_LOG_ERROR, ("Value for %s must be a 4CC, %s is not - please check usage\n", arg, arg_val));
				arg_parse_res = 2;
				return GF_TRUE;
			}
			* (u32 *) arg_desc->arg_ptr = GF_4CC(arg_val[0], arg_val[1], arg_val[2], arg_val[3]);
			return GF_TRUE;
		}

		* (char **) arg_desc->arg_ptr = arg_val;
		return GF_TRUE;
	}
	if (!arg_val) {
		M4_LOG(GF_LOG_ERROR, ("Missing value for %s - please check usage\n", arg));
		arg_parse_res = 2;
		return GF_TRUE;
	}

	if (arg_desc->type == GF_ARG_DOUBLE) {
		Double v = atof(arg_val);
		if (arg_desc->parse_flags & ARG_DIV_1000) {
			v /= 1000;
		}
		if ((arg_desc->parse_flags & ARG_NON_ZERO) && !v) {
			M4_LOG(GF_LOG_ERROR, ("Value for %s shall not be 0 - please check usage\n", arg));
			arg_parse_res = 2;
			return GF_TRUE;
		}
		* (Double *) arg_desc->arg_ptr = v;
		return GF_TRUE;
	}

	if (arg_desc->type != GF_ARG_INT) {
		M4_LOG(GF_LOG_ERROR, ("Unsupported argument type for %s - please report to gpac devs\n", arg));
		arg_parse_res = 2;
		return GF_TRUE;
	}
	if (arg_desc->parse_flags & ARG_64BITS) {
		u64 v;
		sscanf(arg_val, LLU, &v);
		if (arg_desc->parse_flags & ARG_DIV_1000) {
			v /= 1000;
		}
		if ((arg_desc->parse_flags & ARG_NON_ZERO) && !v) {
			M4_LOG(GF_LOG_ERROR, ("Value for %s shall not be 0 - please check usage\n", arg));
			arg_parse_res = 2;
			return GF_TRUE;
		}
		* (u64 *) arg_desc->arg_ptr = v;
	} else {
		u32 v = atoi(arg_val);
		if (arg_desc->parse_flags & ARG_DIV_1000) {
			v /= 1000;
		}
		if ((arg_desc->parse_flags & ARG_NON_ZERO) && !v) {
			M4_LOG(GF_LOG_ERROR, ("Value for %s shall not be 0 - please check usage\n", arg));
			arg_parse_res = 2;
			return GF_TRUE;
		}
		* (s32 *) arg_desc->arg_ptr = v;
	}
	return GF_TRUE;
}