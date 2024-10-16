static void var_free(RAnalVar *var) {
	if (var) {
		r_anal_var_clear_accesses (var);
		r_vector_fini (&var->constraints);
		free (var->name);
		free (var->regname);
		free (var->type);
		free (var->comment);
		free (var);
	}
}