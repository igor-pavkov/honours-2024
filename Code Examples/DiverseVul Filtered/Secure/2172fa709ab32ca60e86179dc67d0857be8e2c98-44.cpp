static void context_struct_compute_av(struct context *scontext,
				      struct context *tcontext,
				      u16 tclass,
				      struct av_decision *avd)
{
	struct constraint_node *constraint;
	struct role_allow *ra;
	struct avtab_key avkey;
	struct avtab_node *node;
	struct class_datum *tclass_datum;
	struct ebitmap *sattr, *tattr;
	struct ebitmap_node *snode, *tnode;
	unsigned int i, j;

	avd->allowed = 0;
	avd->auditallow = 0;
	avd->auditdeny = 0xffffffff;

	if (unlikely(!tclass || tclass > policydb.p_classes.nprim)) {
		if (printk_ratelimit())
			printk(KERN_WARNING "SELinux:  Invalid class %hu\n", tclass);
		return;
	}

	tclass_datum = policydb.class_val_to_struct[tclass - 1];

	/*
	 * If a specific type enforcement rule was defined for
	 * this permission check, then use it.
	 */
	avkey.target_class = tclass;
	avkey.specified = AVTAB_AV;
	sattr = flex_array_get(policydb.type_attr_map_array, scontext->type - 1);
	BUG_ON(!sattr);
	tattr = flex_array_get(policydb.type_attr_map_array, tcontext->type - 1);
	BUG_ON(!tattr);
	ebitmap_for_each_positive_bit(sattr, snode, i) {
		ebitmap_for_each_positive_bit(tattr, tnode, j) {
			avkey.source_type = i + 1;
			avkey.target_type = j + 1;
			for (node = avtab_search_node(&policydb.te_avtab, &avkey);
			     node;
			     node = avtab_search_node_next(node, avkey.specified)) {
				if (node->key.specified == AVTAB_ALLOWED)
					avd->allowed |= node->datum.data;
				else if (node->key.specified == AVTAB_AUDITALLOW)
					avd->auditallow |= node->datum.data;
				else if (node->key.specified == AVTAB_AUDITDENY)
					avd->auditdeny &= node->datum.data;
			}

			/* Check conditional av table for additional permissions */
			cond_compute_av(&policydb.te_cond_avtab, &avkey, avd);

		}
	}

	/*
	 * Remove any permissions prohibited by a constraint (this includes
	 * the MLS policy).
	 */
	constraint = tclass_datum->constraints;
	while (constraint) {
		if ((constraint->permissions & (avd->allowed)) &&
		    !constraint_expr_eval(scontext, tcontext, NULL,
					  constraint->expr)) {
			avd->allowed &= ~(constraint->permissions);
		}
		constraint = constraint->next;
	}

	/*
	 * If checking process transition permission and the
	 * role is changing, then check the (current_role, new_role)
	 * pair.
	 */
	if (tclass == policydb.process_class &&
	    (avd->allowed & policydb.process_trans_perms) &&
	    scontext->role != tcontext->role) {
		for (ra = policydb.role_allow; ra; ra = ra->next) {
			if (scontext->role == ra->role &&
			    tcontext->role == ra->new_role)
				break;
		}
		if (!ra)
			avd->allowed &= ~policydb.process_trans_perms;
	}

	/*
	 * If the given source and target types have boundary
	 * constraint, lazy checks have to mask any violated
	 * permission and notice it to userspace via audit.
	 */
	type_attribute_bounds_av(scontext, tcontext,
				 tclass, avd);
}