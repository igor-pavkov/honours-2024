	ModResult OnUserPreTagMessage(User* user, const MessageTarget& target, CTCTags::TagMessageDetails& details) CXX11_OVERRIDE
	{
		if (target.type == MessageTarget::TYPE_CHANNEL)
			return BuildChannelExempts(user, target.Get<Channel>(), SilenceEntry::SF_TAGMSG_CHANNEL, details.exemptions);

		if (target.type == MessageTarget::TYPE_USER && !CanReceiveMessage(user, target.Get<User>(), SilenceEntry::SF_TAGMSG_USER))
		{
			details.echo_original = true;
			return MOD_RES_DENY;
		}

		return MOD_RES_PASSTHRU;
	}