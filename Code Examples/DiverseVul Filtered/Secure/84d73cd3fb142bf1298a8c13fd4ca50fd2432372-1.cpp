int rtnl_unregister(int protocol, int msgtype)
{
	int msgindex;

	BUG_ON(protocol < 0 || protocol > RTNL_FAMILY_MAX);
	msgindex = rtm_msgindex(msgtype);

	if (rtnl_msg_handlers[protocol] == NULL)
		return -ENOENT;

	rtnl_msg_handlers[protocol][msgindex].doit = NULL;
	rtnl_msg_handlers[protocol][msgindex].dumpit = NULL;

	return 0;
}