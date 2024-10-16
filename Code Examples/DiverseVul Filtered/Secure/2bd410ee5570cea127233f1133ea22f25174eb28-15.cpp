	FOR_EACH_MODULE_Type(CIRCNetwork* pNetwork) : CMuser(pNetwork ? pNetwork->GetUser()->GetModules() : CMtemp), CMnet(pNetwork ? pNetwork->GetModules() : CMtemp) {
		where = AtGlobal;
	}