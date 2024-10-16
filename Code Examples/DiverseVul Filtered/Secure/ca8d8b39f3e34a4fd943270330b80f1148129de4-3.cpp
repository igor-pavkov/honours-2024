static bool is_data_section(RBinSection *sect) {
	if (strstr (sect->name, "_cstring")) {
		return true;
	}
	if (strstr (sect->name, "_objc_methname")) {
		return true;
	}
	if (strstr (sect->name, "_objc_classname")) {
		return true;
	}
	if (strstr (sect->name, "_objc_methtype")) {
		return true;
	}
	return false;
}