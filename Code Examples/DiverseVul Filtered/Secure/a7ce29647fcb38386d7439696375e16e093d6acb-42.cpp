R_API void r_anal_function_delete_all_vars(RAnalFunction *fcn) {
	r_return_if_fail (fcn);
	if (fcn->vars.v.len > 0) {
		void **it;
		r_pvector_foreach (&fcn->vars, it) {
			var_free (*it);
		}
	}
	r_pvector_clear (&fcn->vars);
}