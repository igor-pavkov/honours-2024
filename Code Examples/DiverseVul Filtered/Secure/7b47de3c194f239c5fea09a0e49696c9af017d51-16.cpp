	ModResult OnUserPreMessage(User* user, const MessageTarget& target, MessageDetails& details) CXX11_OVERRIDE
	{
		std::string ctcpname;
		bool is_ctcp = details.IsCTCP(ctcpname) && !irc::equals(ctcpname, "ACTION");

		SilenceEntry::SilenceFlags flag = SilenceEntry::SF_NONE;
		if (target.type == MessageTarget::TYPE_CHANNEL)
		{
			if (is_ctcp)
				flag = SilenceEntry::SF_CTCP_CHANNEL;
			else if (details.type == MSG_NOTICE)
				flag = SilenceEntry::SF_NOTICE_CHANNEL;
			else if (details.type == MSG_PRIVMSG)
				flag = SilenceEntry::SF_PRIVMSG_CHANNEL;

			return BuildChannelExempts(user, target.Get<Channel>(), flag, details.exemptions);
		}

		if (target.type == MessageTarget::TYPE_USER)
		{
			if (is_ctcp)
				flag = SilenceEntry::SF_CTCP_USER;
			else if (details.type == MSG_NOTICE)
				flag = SilenceEntry::SF_NOTICE_USER;
			else if (details.type == MSG_PRIVMSG)
				flag = SilenceEntry::SF_PRIVMSG_USER;

			if (!CanReceiveMessage(user, target.Get<User>(), flag))
			{
				details.echo_original = true;
				return MOD_RES_DENY;
			}
		}

		return MOD_RES_PASSTHRU;
	}