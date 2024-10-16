static RStrBuf *__generic_io_dest(ut8 port, int write, CPU_MODEL *cpu) {
	RStrBuf *r = r_strbuf_new ("");
	CPU_CONST *c = const_by_value (cpu, CPU_CONST_REG, port);
	if (c != NULL) {
		r_strbuf_set (r, c->key);
		if (write) {
			r_strbuf_append (r, ",=");
		}
	} else {
		r_strbuf_setf (r, "_io,%d,+,%s[1]", port, write ? "=" : "");
	}

	return r;
}