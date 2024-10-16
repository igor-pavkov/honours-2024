static void sit_destroy_tunnels(struct sit_net *sitn)
{
	int prio;

	for (prio = 1; prio < 4; prio++) {
		int h;
		for (h = 0; h < HASH_SIZE; h++) {
			struct ip_tunnel *t;
			while ((t = sitn->tunnels[prio][h]) != NULL)
				unregister_netdevice(t->dev);
		}
	}
}