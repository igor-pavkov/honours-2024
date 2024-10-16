static CPU_MODEL *__get_cpu_model_recursive(char *model) {
	CPU_MODEL *cpu = NULL;

	for (cpu = cpu_models; cpu < cpu_models + ((sizeof (cpu_models) / sizeof (CPU_MODEL))) - 1; cpu++) {
		if (!strcasecmp (model, cpu->model)) {
			break;
		}
	}

	// fix inheritance tree
	if (cpu->inherit && !cpu->inherit_cpu_p) {
		cpu->inherit_cpu_p = get_cpu_model (cpu->inherit);
		if (!cpu->inherit_cpu_p) {
			eprintf ("ERROR: Cannot inherit from unknown CPU model '%s'.\n", cpu->inherit);
		}
	}

	return cpu;
}