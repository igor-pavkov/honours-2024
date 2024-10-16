	FOR_EACH_MODULE_Type(std::pair<CUser*, CIRCNetwork*> arg) : CMuser(arg.first ? arg.first->GetModules() : CMtemp), CMnet(arg.second ? arg.second->GetModules() : CMtemp) {
		where = AtGlobal;
	}