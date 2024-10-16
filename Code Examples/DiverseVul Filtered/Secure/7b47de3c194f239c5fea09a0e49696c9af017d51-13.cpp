	CmdResult ShowSilenceList(LocalUser* user)
	{
		SilenceList* list = ext.get(user);
		if (list)
		{
			for (SilenceList::const_iterator iter = list->begin(); iter != list->end(); ++iter)
			{
				user->WriteNumeric(RPL_SILELIST, iter->mask, SilenceEntry::BitsToFlags(iter->flags));
			}
		}
		user->WriteNumeric(RPL_ENDOFSILELIST, "End of SILENCE list");
		return CMD_SUCCESS;
	}