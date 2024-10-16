static const char *get_regname(RAnal *anal, RAnalValue *value) {
	const char *name = NULL;
	if (value && value->reg && value->reg->name) {
		name = value->reg->name;
		RRegItem *ri = r_reg_get (anal->reg, value->reg->name, -1);
		if (ri && (ri->size == 32) && (anal->bits == 64)) {
			name = r_reg_32_to_64 (anal->reg, value->reg->name);
		}
	}
	return name;
}