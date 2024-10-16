static int __init unittest_data_add(void)
{
	void *unittest_data;
	struct device_node *unittest_data_node, *np;
	/*
	 * __dtb_testcases_begin[] and __dtb_testcases_end[] are magically
	 * created by cmd_dt_S_dtb in scripts/Makefile.lib
	 */
	extern uint8_t __dtb_testcases_begin[];
	extern uint8_t __dtb_testcases_end[];
	const int size = __dtb_testcases_end - __dtb_testcases_begin;
	int rc;

	if (!size) {
		pr_warn("%s: No testcase data to attach; not running tests\n",
			__func__);
		return -ENODATA;
	}

	/* creating copy */
	unittest_data = kmemdup(__dtb_testcases_begin, size, GFP_KERNEL);
	if (!unittest_data)
		return -ENOMEM;

	of_fdt_unflatten_tree(unittest_data, NULL, &unittest_data_node);
	if (!unittest_data_node) {
		pr_warn("%s: No tree to attach; not running tests\n", __func__);
		return -ENODATA;
	}

	/*
	 * This lock normally encloses of_resolve_phandles()
	 */
	of_overlay_mutex_lock();

	rc = of_resolve_phandles(unittest_data_node);
	if (rc) {
		pr_err("%s: Failed to resolve phandles (rc=%i)\n", __func__, rc);
		of_overlay_mutex_unlock();
		return -EINVAL;
	}

	if (!of_root) {
		of_root = unittest_data_node;
		for_each_of_allnodes(np)
			__of_attach_node_sysfs(np);
		of_aliases = of_find_node_by_path("/aliases");
		of_chosen = of_find_node_by_path("/chosen");
		of_overlay_mutex_unlock();
		return 0;
	}

	/* attach the sub-tree to live tree */
	np = unittest_data_node->child;
	while (np) {
		struct device_node *next = np->sibling;

		np->parent = of_root;
		attach_node_and_children(np);
		np = next;
	}

	of_overlay_mutex_unlock();

	return 0;
}