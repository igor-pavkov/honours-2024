static int tipc_nl_compat_media_dump(struct tipc_nl_compat_msg *msg,
				     struct nlattr **attrs)
{
	struct nlattr *media[TIPC_NLA_MEDIA_MAX + 1];
	int err;

	if (!attrs[TIPC_NLA_MEDIA])
		return -EINVAL;

	err = nla_parse_nested(media, TIPC_NLA_MEDIA_MAX, attrs[TIPC_NLA_MEDIA],
			       NULL);
	if (err)
		return err;

	return tipc_add_tlv(msg->rep, TIPC_TLV_MEDIA_NAME,
			    nla_data(media[TIPC_NLA_MEDIA_NAME]),
			    nla_len(media[TIPC_NLA_MEDIA_NAME]));
}