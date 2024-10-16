u32 ethtool_op_get_flags(struct net_device *dev)
{
	/* in the future, this function will probably contain additional
	 * handling for flags which are not so easily handled
	 * by a simple masking operation
	 */

	return dev->features & flags_dup_features;
}