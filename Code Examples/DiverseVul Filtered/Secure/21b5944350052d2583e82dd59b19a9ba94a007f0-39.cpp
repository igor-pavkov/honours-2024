static void rtnl_net_notifyid(struct net *net, int cmd, int id)
{
	struct sk_buff *msg;
	int err = -ENOMEM;

	msg = nlmsg_new(rtnl_net_get_size(), GFP_KERNEL);
	if (!msg)
		goto out;

	err = rtnl_net_fill(msg, 0, 0, 0, cmd, net, id);
	if (err < 0)
		goto err_out;

	rtnl_notify(msg, net, 0, RTNLGRP_NSID, NULL, 0);
	return;

err_out:
	nlmsg_free(msg);
out:
	rtnl_set_sk_err(net, RTNLGRP_NSID, err);
}