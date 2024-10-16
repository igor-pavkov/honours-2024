static int get_parent_handle(struct device_node *np)
{
	struct device_node *parent;
	const uint32_t *prop;
	uint32_t handle;
	int len;

	parent = of_get_parent(np);
	if (!parent)
		/* It's not really possible for this to fail */
		return -ENODEV;

	/*
	 * The proper name for the handle property is "hv-handle", but some
	 * older versions of the hypervisor used "reg".
	 */
	prop = of_get_property(parent, "hv-handle", &len);
	if (!prop)
		prop = of_get_property(parent, "reg", &len);

	if (!prop || (len != sizeof(uint32_t))) {
		/* This can happen only if the node is malformed */
		of_node_put(parent);
		return -ENODEV;
	}

	handle = be32_to_cpup(prop);
	of_node_put(parent);

	return handle;
}