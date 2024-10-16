unsigned int ebt_do_table (unsigned int hook, struct sk_buff *skb,
   const struct net_device *in, const struct net_device *out,
   struct ebt_table *table)
{
	int i, nentries;
	struct ebt_entry *point;
	struct ebt_counter *counter_base, *cb_base;
	const struct ebt_entry_target *t;
	int verdict, sp = 0;
	struct ebt_chainstack *cs;
	struct ebt_entries *chaininfo;
	const char *base;
	const struct ebt_table_info *private;
	struct xt_action_param acpar;

	acpar.family  = NFPROTO_BRIDGE;
	acpar.in      = in;
	acpar.out     = out;
	acpar.hotdrop = false;
	acpar.hooknum = hook;

	read_lock_bh(&table->lock);
	private = table->private;
	cb_base = COUNTER_BASE(private->counters, private->nentries,
	   smp_processor_id());
	if (private->chainstack)
		cs = private->chainstack[smp_processor_id()];
	else
		cs = NULL;
	chaininfo = private->hook_entry[hook];
	nentries = private->hook_entry[hook]->nentries;
	point = (struct ebt_entry *)(private->hook_entry[hook]->data);
	counter_base = cb_base + private->hook_entry[hook]->counter_offset;
	/* base for chain jumps */
	base = private->entries;
	i = 0;
	while (i < nentries) {
		if (ebt_basic_match(point, skb, in, out))
			goto letscontinue;

		if (EBT_MATCH_ITERATE(point, ebt_do_match, skb, &acpar) != 0)
			goto letscontinue;
		if (acpar.hotdrop) {
			read_unlock_bh(&table->lock);
			return NF_DROP;
		}

		/* increase counter */
		(*(counter_base + i)).pcnt++;
		(*(counter_base + i)).bcnt += skb->len;

		/* these should only watch: not modify, nor tell us
		   what to do with the packet */
		EBT_WATCHER_ITERATE(point, ebt_do_watcher, skb, &acpar);

		t = (struct ebt_entry_target *)
		   (((char *)point) + point->target_offset);
		/* standard target */
		if (!t->u.target->target)
			verdict = ((struct ebt_standard_target *)t)->verdict;
		else {
			acpar.target   = t->u.target;
			acpar.targinfo = t->data;
			verdict = t->u.target->target(skb, &acpar);
		}
		if (verdict == EBT_ACCEPT) {
			read_unlock_bh(&table->lock);
			return NF_ACCEPT;
		}
		if (verdict == EBT_DROP) {
			read_unlock_bh(&table->lock);
			return NF_DROP;
		}
		if (verdict == EBT_RETURN) {
letsreturn:
#ifdef CONFIG_NETFILTER_DEBUG
			if (sp == 0) {
				BUGPRINT("RETURN on base chain");
				/* act like this is EBT_CONTINUE */
				goto letscontinue;
			}
#endif
			sp--;
			/* put all the local variables right */
			i = cs[sp].n;
			chaininfo = cs[sp].chaininfo;
			nentries = chaininfo->nentries;
			point = cs[sp].e;
			counter_base = cb_base +
			   chaininfo->counter_offset;
			continue;
		}
		if (verdict == EBT_CONTINUE)
			goto letscontinue;
#ifdef CONFIG_NETFILTER_DEBUG
		if (verdict < 0) {
			BUGPRINT("bogus standard verdict\n");
			read_unlock_bh(&table->lock);
			return NF_DROP;
		}
#endif
		/* jump to a udc */
		cs[sp].n = i + 1;
		cs[sp].chaininfo = chaininfo;
		cs[sp].e = ebt_next_entry(point);
		i = 0;
		chaininfo = (struct ebt_entries *) (base + verdict);
#ifdef CONFIG_NETFILTER_DEBUG
		if (chaininfo->distinguisher) {
			BUGPRINT("jump to non-chain\n");
			read_unlock_bh(&table->lock);
			return NF_DROP;
		}
#endif
		nentries = chaininfo->nentries;
		point = (struct ebt_entry *)chaininfo->data;
		counter_base = cb_base + chaininfo->counter_offset;
		sp++;
		continue;
letscontinue:
		point = ebt_next_entry(point);
		i++;
	}

	/* I actually like this :) */
	if (chaininfo->policy == EBT_RETURN)
		goto letsreturn;
	if (chaininfo->policy == EBT_ACCEPT) {
		read_unlock_bh(&table->lock);
		return NF_ACCEPT;
	}
	read_unlock_bh(&table->lock);
	return NF_DROP;
}