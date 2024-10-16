inline bool FOR_EACH_MODULE_CanContinue(FOR_EACH_MODULE_Type& state, CModules::iterator& i) {
	if (state.where == FOR_EACH_MODULE_Type::AtGlobal && i == CZNC::Get().GetModules().end()) {
		i = state.CMuser.begin();
		state.where = FOR_EACH_MODULE_Type::AtUser;
	}
	if (state.where == FOR_EACH_MODULE_Type::AtUser && i == state.CMuser.end()) {
		i = state.CMnet.begin();
		state.where = FOR_EACH_MODULE_Type::AtNetwork;
	}
	return !(state.where == FOR_EACH_MODULE_Type::AtNetwork && i == state.CMnet.end());
}