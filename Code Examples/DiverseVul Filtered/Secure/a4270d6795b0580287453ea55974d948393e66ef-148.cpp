#if IS_ENABLED(CONFIG_CGROUP_NET_PRIO)
static void skb_update_prio(struct sk_buff *skb)
{
	const struct netprio_map *map;
	const struct sock *sk;
	unsigned int prioidx;

	if (skb->priority)
		return;
	map = rcu_dereference_bh(skb->dev->priomap);
	if (!map)
		return;
	sk = skb_to_full_sk(skb);
	if (!sk)
		return;

	prioidx = sock_cgroup_prioidx(&sk->sk_cgrp_data);

	if (prioidx < map->priomap_len)
		skb->priority = map->priomap[prioidx];