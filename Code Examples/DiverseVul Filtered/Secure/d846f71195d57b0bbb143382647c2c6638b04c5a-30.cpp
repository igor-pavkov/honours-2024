static int translate_table(struct net *net, const char *name,
			   struct ebt_table_info *newinfo)
{
	unsigned int i, j, k, udc_cnt;
	int ret;
	struct ebt_cl_stack *cl_s = NULL; /* used in the checking for chain loops */

	i = 0;
	while (i < NF_BR_NUMHOOKS && !newinfo->hook_entry[i])
		i++;
	if (i == NF_BR_NUMHOOKS) {
		BUGPRINT("No valid hooks specified\n");
		return -EINVAL;
	}
	if (newinfo->hook_entry[i] != (struct ebt_entries *)newinfo->entries) {
		BUGPRINT("Chains don't start at beginning\n");
		return -EINVAL;
	}
	/* make sure chains are ordered after each other in same order
	   as their corresponding hooks */
	for (j = i + 1; j < NF_BR_NUMHOOKS; j++) {
		if (!newinfo->hook_entry[j])
			continue;
		if (newinfo->hook_entry[j] <= newinfo->hook_entry[i]) {
			BUGPRINT("Hook order must be followed\n");
			return -EINVAL;
		}
		i = j;
	}

	/* do some early checkings and initialize some things */
	i = 0; /* holds the expected nr. of entries for the chain */
	j = 0; /* holds the up to now counted entries for the chain */
	k = 0; /* holds the total nr. of entries, should equal
		  newinfo->nentries afterwards */
	udc_cnt = 0; /* will hold the nr. of user defined chains (udc) */
	ret = EBT_ENTRY_ITERATE(newinfo->entries, newinfo->entries_size,
	   ebt_check_entry_size_and_hooks, newinfo,
	   &i, &j, &k, &udc_cnt);

	if (ret != 0)
		return ret;

	if (i != j) {
		BUGPRINT("nentries does not equal the nr of entries in the "
			 "(last) chain\n");
		return -EINVAL;
	}
	if (k != newinfo->nentries) {
		BUGPRINT("Total nentries is wrong\n");
		return -EINVAL;
	}

	/* get the location of the udc, put them in an array
	   while we're at it, allocate the chainstack */
	if (udc_cnt) {
		/* this will get free'd in do_replace()/ebt_register_table()
		   if an error occurs */
		newinfo->chainstack =
			vmalloc(nr_cpu_ids * sizeof(*(newinfo->chainstack)));
		if (!newinfo->chainstack)
			return -ENOMEM;
		for_each_possible_cpu(i) {
			newinfo->chainstack[i] =
			  vmalloc(udc_cnt * sizeof(*(newinfo->chainstack[0])));
			if (!newinfo->chainstack[i]) {
				while (i)
					vfree(newinfo->chainstack[--i]);
				vfree(newinfo->chainstack);
				newinfo->chainstack = NULL;
				return -ENOMEM;
			}
		}

		cl_s = vmalloc(udc_cnt * sizeof(*cl_s));
		if (!cl_s)
			return -ENOMEM;
		i = 0; /* the i'th udc */
		EBT_ENTRY_ITERATE(newinfo->entries, newinfo->entries_size,
		   ebt_get_udc_positions, newinfo, &i, cl_s);
		/* sanity check */
		if (i != udc_cnt) {
			BUGPRINT("i != udc_cnt\n");
			vfree(cl_s);
			return -EFAULT;
		}
	}

	/* Check for loops */
	for (i = 0; i < NF_BR_NUMHOOKS; i++)
		if (newinfo->hook_entry[i])
			if (check_chainloops(newinfo->hook_entry[i],
			   cl_s, udc_cnt, i, newinfo->entries)) {
				vfree(cl_s);
				return -EINVAL;
			}

	/* we now know the following (along with E=mc²):
	   - the nr of entries in each chain is right
	   - the size of the allocated space is right
	   - all valid hooks have a corresponding chain
	   - there are no loops
	   - wrong data can still be on the level of a single entry
	   - could be there are jumps to places that are not the
	     beginning of a chain. This can only occur in chains that
	     are not accessible from any base chains, so we don't care. */

	/* used to know what we need to clean up if something goes wrong */
	i = 0;
	ret = EBT_ENTRY_ITERATE(newinfo->entries, newinfo->entries_size,
	   ebt_check_entry, net, newinfo, name, &i, cl_s, udc_cnt);
	if (ret != 0) {
		EBT_ENTRY_ITERATE(newinfo->entries, newinfo->entries_size,
				  ebt_cleanup_entry, net, &i);
	}
	vfree(cl_s);
	return ret;
}