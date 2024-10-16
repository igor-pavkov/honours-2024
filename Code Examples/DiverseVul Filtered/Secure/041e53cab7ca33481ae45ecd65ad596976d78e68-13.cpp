static int __esil_pop_argument(RAnalEsil *esil, ut64 *v) {
	char *t = r_anal_esil_pop (esil);
	if (!t || !r_anal_esil_get_parm (esil, t, v)) {
		free (t);
		return false;
	}
	free (t);
	return true;
}