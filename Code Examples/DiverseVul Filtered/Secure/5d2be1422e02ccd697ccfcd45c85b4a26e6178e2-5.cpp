static int tipc_nl_compat_media_set(struct sk_buff *skb,
				    struct tipc_nl_compat_msg *msg)
{
	struct nlattr *prop;
	struct nlattr *media;
	struct tipc_link_config *lc;

	lc = (struct tipc_link_config *)TLV_DATA(msg->req);

	media = nla_nest_start(skb, TIPC_NLA_MEDIA);
	if (!media)
		return -EMSGSIZE;

	if (nla_put_string(skb, TIPC_NLA_MEDIA_NAME, lc->name))
		return -EMSGSIZE;

	prop = nla_nest_start(skb, TIPC_NLA_MEDIA_PROP);
	if (!prop)
		return -EMSGSIZE;

	__tipc_add_link_prop(skb, msg, lc);
	nla_nest_end(skb, prop);
	nla_nest_end(skb, media);

	return 0;
}