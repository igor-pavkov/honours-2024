	void ReadConfig(ConfigStatus& status) CXX11_OVERRIDE
	{
		ConfigTag* tag = ServerInstance->Config->ConfValue("silence");
		exemptuline = tag->getBool("exemptuline", true);
		cmd.maxsilence = tag->getUInt("maxentries", 32, 1);
	}