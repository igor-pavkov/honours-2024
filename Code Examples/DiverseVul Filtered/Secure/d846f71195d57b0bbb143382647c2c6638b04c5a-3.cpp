static int ebt_verify_pointers(const struct ebt_replace *repl,
			       struct ebt_table_info *newinfo)
{
	unsigned int limit = repl->entries_size;
	unsigned int valid_hooks = repl->valid_hooks;
	unsigned int offset = 0;
	int i;

	for (i = 0; i < NF_BR_NUMHOOKS; i++)
		newinfo->hook_entry[i] = NULL;

	newinfo->entries_size = repl->entries_size;
	newinfo->nentries = repl->nentries;

	while (offset < limit) {
		size_t left = limit - offset;
		struct ebt_entry *e = (void *)newinfo->entries + offset;

		if (left < sizeof(unsigned int))
			break;

		for (i = 0; i < NF_BR_NUMHOOKS; i++) {
			if ((valid_hooks & (1 << i)) == 0)
				continue;
			if ((char __user *)repl->hook_entry[i] ==
			     repl->entries + offset)
				break;
		}

		if (i != NF_BR_NUMHOOKS || !(e->bitmask & EBT_ENTRY_OR_ENTRIES)) {
			if (e->bitmask != 0) {
				/* we make userspace set this right,
				   so there is no misunderstanding */
				BUGPRINT("EBT_ENTRY_OR_ENTRIES shouldn't be set "
					 "in distinguisher\n");
				return -EINVAL;
			}
			if (i != NF_BR_NUMHOOKS)
				newinfo->hook_entry[i] = (struct ebt_entries *)e;
			if (left < sizeof(struct ebt_entries))
				break;
			offset += sizeof(struct ebt_entries);
		} else {
			if (left < sizeof(struct ebt_entry))
				break;
			if (left < e->next_offset)
				break;
			if (e->next_offset < sizeof(struct ebt_entry))
				return -EINVAL;
			offset += e->next_offset;
		}
	}
	if (offset != limit) {
		BUGPRINT("entries_size too small\n");
		return -EINVAL;
	}

	/* check if all valid hooks have a chain */
	for (i = 0; i < NF_BR_NUMHOOKS; i++) {
		if (!newinfo->hook_entry[i] &&
		   (valid_hooks & (1 << i))) {
			BUGPRINT("Valid hook without chain\n");
			return -EINVAL;
		}
	}
	return 0;
}