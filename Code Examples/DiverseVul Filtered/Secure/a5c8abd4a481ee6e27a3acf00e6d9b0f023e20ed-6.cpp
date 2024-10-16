int parse_sar_I_opt(char *argv[], int *opt, uint64_t *flags, struct activity *act[])
{
	int p;

	/* Select interrupt activity */
	p = get_activity_position(act, A_IRQ, EXIT_IF_NOT_FOUND);
	act[p]->options |= AO_SELECTED;

	if (argv[++(*opt)]) {
		if (parse_values(argv[*opt], act[p]->bitmap->b_array,
			     act[p]->bitmap->b_size, K_SUM))
			return 1;
		(*opt)++;
		*flags |= S_F_OPTION_I;
		return 0;
	}

	return 1;
}