	ModResult OnUserPreInvite(User* source, User* dest, Channel* channel, time_t timeout) CXX11_OVERRIDE
	{
		return CanReceiveMessage(source, dest, SilenceEntry::SF_INVITE) ? MOD_RES_PASSTHRU : MOD_RES_DENY;
	}