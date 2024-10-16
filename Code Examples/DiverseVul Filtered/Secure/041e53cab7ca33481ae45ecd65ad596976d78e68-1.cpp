INST_HANDLER (des) {	// DES k
	if (desctx.round < 16) {	//DES
		op->type = R_ANAL_OP_TYPE_CRYPTO;
		op->cycles = 1;		//redo this
		r_strbuf_setf (&op->esil, "%d,des", desctx.round);
	}
}