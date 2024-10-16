static void __exit ipddp_cleanup_module(void)
{
        struct ipddp_route *p;

	unregister_netdev(dev_ipddp);
        free_netdev(dev_ipddp);

        while (ipddp_route_list) {
                p = ipddp_route_list->next;
                kfree(ipddp_route_list);
                ipddp_route_list = p;
        }
}