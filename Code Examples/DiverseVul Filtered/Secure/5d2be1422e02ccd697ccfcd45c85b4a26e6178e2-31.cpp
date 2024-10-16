static int tipc_nl_compat_publ_dump(struct tipc_nl_compat_msg *msg, u32 sock)
{
	int err;
	void *hdr;
	struct nlattr *nest;
	struct sk_buff *args;
	struct tipc_nl_compat_cmd_dump dump;

	args = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!args)
		return -ENOMEM;

	hdr = genlmsg_put(args, 0, 0, &tipc_genl_family, NLM_F_MULTI,
			  TIPC_NL_PUBL_GET);

	nest = nla_nest_start(args, TIPC_NLA_SOCK);
	if (!nest) {
		kfree_skb(args);
		return -EMSGSIZE;
	}

	if (nla_put_u32(args, TIPC_NLA_SOCK_REF, sock)) {
		kfree_skb(args);
		return -EMSGSIZE;
	}

	nla_nest_end(args, nest);
	genlmsg_end(args, hdr);

	dump.dumpit = tipc_nl_publ_dump;
	dump.format = __tipc_nl_compat_publ_dump;

	err = __tipc_nl_compat_dumpit(&dump, msg, args);

	kfree_skb(args);

	return err;
}