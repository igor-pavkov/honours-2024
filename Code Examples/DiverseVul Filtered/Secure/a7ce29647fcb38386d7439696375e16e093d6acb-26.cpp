R_API void r_anal_var_add_constraint(RAnalVar *var, R_BORROW RAnalVarConstraint *constraint) {
	r_vector_push (&var->constraints, constraint);
}