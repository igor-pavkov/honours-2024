	CmdResult AddSilence(LocalUser* user, const std::string& mask, uint32_t flags)
	{
		SilenceList* list = ext.get(user);
		if (list && list->size() > maxsilence)
		{
			user->WriteNumeric(ERR_SILELISTFULL, mask, SilenceEntry::BitsToFlags(flags), "Your SILENCE list is full");
			return CMD_FAILURE;
		}
		else if (!list)
		{
			// There is no list; create it.
			list = new SilenceList();
			ext.set(user, list);
		}

		if (!list->insert(SilenceEntry(flags, mask)).second)
		{
			user->WriteNumeric(ERR_SILENCE, mask, SilenceEntry::BitsToFlags(flags), "The SILENCE entry you specified already exists");
			return CMD_FAILURE;
		}

		SilenceMessage msg("+" + mask, SilenceEntry::BitsToFlags(flags));
		user->Send(msgprov, msg);
		return CMD_SUCCESS;
	}