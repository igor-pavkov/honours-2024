	FOR_EACH_MODULE_Type(CUser* pUser) : CMuser(pUser ? pUser->GetModules() : CMtemp), CMnet(CMtemp) {
		where = AtGlobal;
	}