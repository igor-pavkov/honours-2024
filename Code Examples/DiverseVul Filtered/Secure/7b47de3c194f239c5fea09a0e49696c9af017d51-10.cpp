	CommandSilence(Module* Creator)
		: SplitCommand(Creator, "SILENCE")
		, msgprov(Creator, "SILENCE")
		, ext("silence_list", ExtensionItem::EXT_USER, Creator)
	{
		allow_empty_last_param = false;
		syntax = "[(+|-)<mask> [CcdiNnPpTtx]]";
	}