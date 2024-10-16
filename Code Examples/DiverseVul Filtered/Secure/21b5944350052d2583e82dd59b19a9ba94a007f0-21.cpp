static void ops_free(const struct pernet_operations *ops, struct net *net)
{
	if (ops->id && ops->size) {
		kfree(net_generic(net, *ops->id));
	}
}