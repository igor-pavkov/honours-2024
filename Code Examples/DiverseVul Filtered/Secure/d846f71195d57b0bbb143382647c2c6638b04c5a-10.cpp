static int check_chainloops(const struct ebt_entries *chain, struct ebt_cl_stack *cl_s,
   unsigned int udc_cnt, unsigned int hooknr, char *base)
{
	int i, chain_nr = -1, pos = 0, nentries = chain->nentries, verdict;
	const struct ebt_entry *e = (struct ebt_entry *)chain->data;
	const struct ebt_entry_target *t;

	while (pos < nentries || chain_nr != -1) {
		/* end of udc, go back one 'recursion' step */
		if (pos == nentries) {
			/* put back values of the time when this chain was called */
			e = cl_s[chain_nr].cs.e;
			if (cl_s[chain_nr].from != -1)
				nentries =
				cl_s[cl_s[chain_nr].from].cs.chaininfo->nentries;
			else
				nentries = chain->nentries;
			pos = cl_s[chain_nr].cs.n;
			/* make sure we won't see a loop that isn't one */
			cl_s[chain_nr].cs.n = 0;
			chain_nr = cl_s[chain_nr].from;
			if (pos == nentries)
				continue;
		}
		t = (struct ebt_entry_target *)
		   (((char *)e) + e->target_offset);
		if (strcmp(t->u.name, EBT_STANDARD_TARGET))
			goto letscontinue;
		if (e->target_offset + sizeof(struct ebt_standard_target) >
		   e->next_offset) {
			BUGPRINT("Standard target size too big\n");
			return -1;
		}
		verdict = ((struct ebt_standard_target *)t)->verdict;
		if (verdict >= 0) { /* jump to another chain */
			struct ebt_entries *hlp2 =
			   (struct ebt_entries *)(base + verdict);
			for (i = 0; i < udc_cnt; i++)
				if (hlp2 == cl_s[i].cs.chaininfo)
					break;
			/* bad destination or loop */
			if (i == udc_cnt) {
				BUGPRINT("bad destination\n");
				return -1;
			}
			if (cl_s[i].cs.n) {
				BUGPRINT("loop\n");
				return -1;
			}
			if (cl_s[i].hookmask & (1 << hooknr))
				goto letscontinue;
			/* this can't be 0, so the loop test is correct */
			cl_s[i].cs.n = pos + 1;
			pos = 0;
			cl_s[i].cs.e = ebt_next_entry(e);
			e = (struct ebt_entry *)(hlp2->data);
			nentries = hlp2->nentries;
			cl_s[i].from = chain_nr;
			chain_nr = i;
			/* this udc is accessible from the base chain for hooknr */
			cl_s[i].hookmask |= (1 << hooknr);
			continue;
		}
letscontinue:
		e = ebt_next_entry(e);
		pos++;
	}
	return 0;
}