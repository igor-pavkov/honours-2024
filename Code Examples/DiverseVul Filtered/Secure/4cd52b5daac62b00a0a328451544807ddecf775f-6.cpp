void jpc_quantize(jas_matrix_t *data, jpc_fix_t stepsize)
{
	jpc_fix_t t;

	if (stepsize == jpc_inttofix(1)) {
		return;
	}

	for (jas_matind_t i = 0; i < jas_matrix_numrows(data); ++i) {
		for (jas_matind_t j = 0; j < jas_matrix_numcols(data); ++j) {
			t = jas_matrix_get(data, i, j);

{
	if (t < 0) {
		t = jpc_fix_neg(jpc_fix_div(jpc_fix_neg(t), stepsize));
	} else {
		t = jpc_fix_div(t, stepsize);
	}
}

			jas_matrix_set(data, i, j, t);
		}
	}
}