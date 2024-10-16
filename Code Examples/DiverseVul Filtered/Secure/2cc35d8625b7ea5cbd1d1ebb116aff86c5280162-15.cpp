	SQLConnection(Module* p, ConfigTag* tag) : SQLProvider(p, "SQL/" + tag->getString("id")),
		config(tag), connection(NULL)
	{
	}