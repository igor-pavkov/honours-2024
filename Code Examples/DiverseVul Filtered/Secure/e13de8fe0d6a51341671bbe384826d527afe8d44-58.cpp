static __init void of_unittest_overlay_high_level(void)
{
	struct device_node *last_sibling;
	struct device_node *np;
	struct device_node *of_symbols;
	struct device_node *overlay_base_symbols;
	struct device_node **pprev;
	struct property *prop;

	if (!overlay_base_root) {
		unittest(0, "overlay_base_root not initialized\n");
		return;
	}

	/*
	 * Could not fixup phandles in unittest_unflatten_overlay_base()
	 * because kmalloc() was not yet available.
	 */
	of_overlay_mutex_lock();
	of_resolve_phandles(overlay_base_root);
	of_overlay_mutex_unlock();


	/*
	 * do not allow overlay_base to duplicate any node already in
	 * tree, this greatly simplifies the code
	 */

	/*
	 * remove overlay_base_root node "__local_fixups", after
	 * being used by of_resolve_phandles()
	 */
	pprev = &overlay_base_root->child;
	for (np = overlay_base_root->child; np; np = np->sibling) {
		if (of_node_name_eq(np, "__local_fixups__")) {
			*pprev = np->sibling;
			break;
		}
		pprev = &np->sibling;
	}

	/* remove overlay_base_root node "__symbols__" if in live tree */
	of_symbols = of_get_child_by_name(of_root, "__symbols__");
	if (of_symbols) {
		/* will have to graft properties from node into live tree */
		pprev = &overlay_base_root->child;
		for (np = overlay_base_root->child; np; np = np->sibling) {
			if (of_node_name_eq(np, "__symbols__")) {
				overlay_base_symbols = np;
				*pprev = np->sibling;
				break;
			}
			pprev = &np->sibling;
		}
	}

	for_each_child_of_node(overlay_base_root, np) {
		struct device_node *base_child;
		for_each_child_of_node(of_root, base_child) {
			if (!strcmp(np->full_name, base_child->full_name)) {
				unittest(0, "illegal node name in overlay_base %pOFn",
					 np);
				return;
			}
		}
	}

	/*
	 * overlay 'overlay_base' is not allowed to have root
	 * properties, so only need to splice nodes into main device tree.
	 *
	 * root node of *overlay_base_root will not be freed, it is lost
	 * memory.
	 */

	for (np = overlay_base_root->child; np; np = np->sibling)
		np->parent = of_root;

	mutex_lock(&of_mutex);

	for (last_sibling = np = of_root->child; np; np = np->sibling)
		last_sibling = np;

	if (last_sibling)
		last_sibling->sibling = overlay_base_root->child;
	else
		of_root->child = overlay_base_root->child;

	for_each_of_allnodes_from(overlay_base_root, np)
		__of_attach_node_sysfs(np);

	if (of_symbols) {
		struct property *new_prop;
		for_each_property_of_node(overlay_base_symbols, prop) {

			new_prop = __of_prop_dup(prop, GFP_KERNEL);
			if (!new_prop) {
				unittest(0, "__of_prop_dup() of '%s' from overlay_base node __symbols__",
					 prop->name);
				goto err_unlock;
			}
			if (__of_add_property(of_symbols, new_prop)) {
				/* "name" auto-generated by unflatten */
				if (!strcmp(new_prop->name, "name"))
					continue;
				unittest(0, "duplicate property '%s' in overlay_base node __symbols__",
					 prop->name);
				goto err_unlock;
			}
			if (__of_add_property_sysfs(of_symbols, new_prop)) {
				unittest(0, "unable to add property '%s' in overlay_base node __symbols__ to sysfs",
					 prop->name);
				goto err_unlock;
			}
		}
	}

	mutex_unlock(&of_mutex);


	/* now do the normal overlay usage test */

	unittest(overlay_data_apply("overlay", NULL),
		 "Adding overlay 'overlay' failed\n");

	unittest(overlay_data_apply("overlay_bad_add_dup_node", NULL),
		 "Adding overlay 'overlay_bad_add_dup_node' failed\n");

	unittest(overlay_data_apply("overlay_bad_add_dup_prop", NULL),
		 "Adding overlay 'overlay_bad_add_dup_prop' failed\n");

	unittest(overlay_data_apply("overlay_bad_phandle", NULL),
		 "Adding overlay 'overlay_bad_phandle' failed\n");

	unittest(overlay_data_apply("overlay_bad_symbol", NULL),
		 "Adding overlay 'overlay_bad_symbol' failed\n");

	return;

err_unlock:
	mutex_unlock(&of_mutex);
}