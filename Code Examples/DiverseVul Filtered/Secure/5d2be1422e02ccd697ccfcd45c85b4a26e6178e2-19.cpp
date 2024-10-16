static int tipc_nl_compat_link_set(struct tipc_nl_compat_cmd_doit *cmd,
				   struct sk_buff *skb,
				   struct tipc_nl_compat_msg *msg)
{
	struct tipc_link_config *lc;
	struct tipc_bearer *bearer;
	struct tipc_media *media;

	lc = (struct tipc_link_config *)TLV_DATA(msg->req);

	media = tipc_media_find(lc->name);
	if (media) {
		cmd->doit = &tipc_nl_media_set;
		return tipc_nl_compat_media_set(skb, msg);
	}

	bearer = tipc_bearer_find(msg->net, lc->name);
	if (bearer) {
		cmd->doit = &tipc_nl_bearer_set;
		return tipc_nl_compat_bearer_set(skb, msg);
	}

	return __tipc_nl_compat_link_set(skb, msg);
}