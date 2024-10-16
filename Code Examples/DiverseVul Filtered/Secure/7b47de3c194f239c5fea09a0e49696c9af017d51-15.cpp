	ModResult BuildChannelExempts(User* source, Channel* channel, SilenceEntry::SilenceFlags flag, CUList& exemptions)
	{
		const Channel::MemberMap& members = channel->GetUsers();
		for (Channel::MemberMap::const_iterator member = members.begin(); member != members.end(); ++member)
		{
			if (!CanReceiveMessage(source, member->first, flag))
				exemptions.insert(member->first);
		}
		return MOD_RES_PASSTHRU;
	}