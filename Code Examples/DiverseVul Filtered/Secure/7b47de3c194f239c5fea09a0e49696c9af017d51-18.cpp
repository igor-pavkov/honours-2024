	bool CanReceiveMessage(User* source, User* target, SilenceEntry::SilenceFlags flag)
	{
		// Servers handle their own clients.
		if (!IS_LOCAL(target))
			return true;

		if (exemptuline && source->server->IsULine())
			return true;

		SilenceList* list = cmd.ext.get(target);
		if (!list)
			return true;

		for (SilenceList::iterator iter = list->begin(); iter != list->end(); ++iter)
		{
			if (!(iter->flags & flag))
				continue;

			if (InspIRCd::Match(source->GetFullHost(), iter->mask))
				return iter->flags & SilenceEntry::SF_EXEMPT;
		}

		return true;
	}