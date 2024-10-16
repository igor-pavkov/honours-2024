static CPU_MODEL *get_cpu_model(char *model) {
	static CPU_MODEL *cpu = NULL;

	// cached value?
	if (cpu && !strcasecmp (model, cpu->model))
		return cpu;

	// do the real search
	cpu = __get_cpu_model_recursive (model);

	return cpu;
}