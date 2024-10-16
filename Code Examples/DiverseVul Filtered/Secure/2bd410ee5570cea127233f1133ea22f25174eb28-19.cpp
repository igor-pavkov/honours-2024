	MODCONSTRUCTOR(CWebAdminMod) {
		VPair vParams;
		vParams.push_back(make_pair("user", ""));
		AddSubPage(new CWebSubPage("settings", "Global Settings", CWebSubPage::F_ADMIN));
		AddSubPage(new CWebSubPage("edituser", "Your Settings", vParams));
		AddSubPage(new CWebSubPage("traffic", "Traffic Info", CWebSubPage::F_ADMIN));
		AddSubPage(new CWebSubPage("listusers", "List Users", CWebSubPage::F_ADMIN));
		AddSubPage(new CWebSubPage("adduser", "Add User", CWebSubPage::F_ADMIN));
	}