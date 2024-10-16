static void __generic_sub_update_flags(RAnalOp *op, char t_d, ut64 v_d, char t_rk, ut64 v_rk, int carry) {
	RStrBuf *d_strbuf, *rk_strbuf;
	char *d, *rk;

	d_strbuf = r_strbuf_new (NULL);
	rk_strbuf = r_strbuf_new (NULL);
	r_strbuf_setf (d_strbuf,  t_d  == 'r' ? "r%d" : "%" PFMT64d, v_d);
	r_strbuf_setf (rk_strbuf, t_rk == 'r' ? "r%d" : "%" PFMT64d, v_rk);

	d = r_strbuf_get(d_strbuf);
	rk = r_strbuf_get(rk_strbuf);

	ESIL_A ("%s,0x08,&,!,"   "%s,0x08,&,!,!,"      "&,"	// H
		"%s,0x08,&,!,!," "0,RPICK,0x08,&,!,!," "&,"
		"%s,0x08,&,!,"   "0,RPICK,0x08,&,!,!," "&,"
		"|,|,hf,=,",
		d, rk, rk, d);
	ESIL_A ("%s,0x80,&,!,!," "%s,0x80,&,!,"        "&,"	// V
		""               "0,RPICK,0x80,&,!,"   "&,"
		"%s,0x80,&,!,"   "%s,0x80,&,!,!,"      "&,"
		""               "0,RPICK,0x80,&,!,!," "&,"
		"|,vf,=,",
		d, rk, d, rk);
	ESIL_A ("0,RPICK,0x80,&,!,!,nf,=,");			// N
	if (carry)
		ESIL_A ("0,RPICK,!,zf,&,zf,=,");		// Z
	else
		ESIL_A ("0,RPICK,!,zf,=,");			// Z
	ESIL_A ("%s,0x80,&,!,"   "%s,0x80,&,!,!,"      "&,"	// C
		"%s,0x80,&,!,!," "0,RPICK,0x80,&,!,!," "&,"
		"%s,0x80,&,!,"   "0,RPICK,0x80,&,!,!," "&,"
		"|,|,cf,=,",
		d, rk, rk, d);
	ESIL_A ("vf,nf,^,sf,=,");				// S

	r_strbuf_free (d_strbuf);
	r_strbuf_free (rk_strbuf);
}