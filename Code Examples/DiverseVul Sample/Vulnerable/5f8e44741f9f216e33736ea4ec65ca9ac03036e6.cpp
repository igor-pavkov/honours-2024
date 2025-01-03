static int rtnl_fill_link_ifmap(struct sk_buff *skb, struct net_device *dev)
{
	struct rtnl_link_ifmap map = {
		.mem_start   = dev->mem_start,
		.mem_end     = dev->mem_end,
		.base_addr   = dev->base_addr,
		.irq         = dev->irq,
		.dma         = dev->dma,
		.port        = dev->if_port,
	};
	if (nla_put(skb, IFLA_MAP, sizeof(map), &map))
		return -EMSGSIZE;

	return 0;
}