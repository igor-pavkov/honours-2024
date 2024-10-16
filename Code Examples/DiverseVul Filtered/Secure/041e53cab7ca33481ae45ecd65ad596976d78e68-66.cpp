static CPU_CONST *const_by_name(CPU_MODEL *cpu, int type, char *c) {
	CPU_CONST **clist, *citem;

	for (clist = cpu->consts; *clist; clist++) {
		for (citem = *clist; citem->key; citem++) {
			if (!strcmp (c, citem->key)
			&& (type == CPU_CONST_NONE || type == citem->type)) {
				return citem;
			}
		}
	}
	if (cpu->inherit_cpu_p)
		return const_by_name (cpu->inherit_cpu_p, type, c);
	eprintf ("ERROR: CONSTANT key[%s] NOT FOUND.\n", c);
	return NULL;
}