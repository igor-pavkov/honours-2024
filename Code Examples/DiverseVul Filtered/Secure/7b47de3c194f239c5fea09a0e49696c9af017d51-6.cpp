	CmdResult RemoveSilence(LocalUser* user, const std::string& mask, uint32_t flags)
	{
		SilenceList* list = ext.get(user);
		if (list)
		{
			for (SilenceList::iterator iter = list->begin(); iter != list->end(); ++iter)
			{
				if (!irc::equals(iter->mask, mask) || iter->flags != flags)
					continue;

				list->erase(iter);
				SilenceMessage msg("-" + mask, SilenceEntry::BitsToFlags(flags));
				user->Send(msgprov, msg);
				return CMD_SUCCESS;
			}
		}

		user->WriteNumeric(ERR_SILENCE, mask, SilenceEntry::BitsToFlags(flags), "The SILENCE entry you specified could not be found");
		return CMD_FAILURE;
	}