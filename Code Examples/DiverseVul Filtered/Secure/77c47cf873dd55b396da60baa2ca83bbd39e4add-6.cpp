static int first_nibble_is_c(RAnal* anal, RAnalOp* op, ut16 code){
	if (IS_TRAP(code)) {
		op->type = R_ANAL_OP_TYPE_SWI;
		op->val = (ut8)(code&0xFF);
	} else if (IS_MOVA_PCREL_R0(code)) {
		// 11000111i8p4.... mova @(<disp>,PC),R0
		op->type = R_ANAL_OP_TYPE_LEA;
		op->src[0] = anal_pcrel_disp_mov (anal, op, code&0xFF, LONG_SIZE);	//this is wrong !
		op->dst = anal_fill_ai_rg (anal, 0); //Always R0
	} else if (IS_BINLOGIC_IMM_R0(code)) {	// 110010__i8 (binop) #imm, R0
		op->src[0] = anal_fill_im (anal, code&0xFF);
		op->src[1] = anal_fill_ai_rg (anal, 0);	//Always R0
		op->dst = anal_fill_ai_rg (anal, 0); //Always R0 except tst #imm, R0
		switch (code & 0xFF00) {
		case 0xC800:	//tst
			//TODO : get correct op->dst ! (T flag)
			op->type = R_ANAL_OP_TYPE_ACMP;
			break;
		case 0xC900:	//and
			op->type = R_ANAL_OP_TYPE_AND;
			break;
		case 0xCA00:	//xor
			op->type = R_ANAL_OP_TYPE_XOR;
			break;
		case 0xCB00:	//or
			op->type = R_ANAL_OP_TYPE_OR;
			break;
		}
	} else if (IS_BINLOGIC_IMM_GBR(code)) {	//110011__i8 (binop).b #imm, @(R0,GBR)
		op->src[0] = anal_fill_im (anal, code&0xFF);
		switch (code & 0xFF00) {
		case 0xCC00:	//tst
			//TODO : get correct op->dst ! (T flag)
			op->type = R_ANAL_OP_TYPE_ACMP;
			break;
		case 0xCD00:	//and
			op->type = R_ANAL_OP_TYPE_AND;
			break;
		case 0xCE00:	//xor
			op->type = R_ANAL_OP_TYPE_XOR;
			break;
		case 0xCF00:	//or
			op->type = R_ANAL_OP_TYPE_OR;
			break;
		}
		//TODO : implement @(R0,GBR) dest and src[1]
	} else if (IS_MOVB_R0_GBRREF(code)) {	//11000000i8*1.... mov.b R0,@(<disp>,GBR)
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, 0);
		//todo: implement @(disp,GBR) dest
	} else if (IS_MOVW_R0_GBRREF(code)) {	//11000001i8*2.... mov.w R0,@(<disp>,GBR)
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, 0);
		//todo: implement @(disp,GBR) dest
	} else if (IS_MOVL_R0_GBRREF(code)) {	//11000010i8*4.... mov.l R0,@(<disp>,GBR)
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, 0);
		//todo: implement @(disp,GBR) dest
	} else if (IS_MOVB_GBRREF_R0(code)) {	//11000100i8*1.... mov.b @(<disp>,GBR),R0
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->dst = anal_fill_ai_rg (anal, 0);
		//todo: implement @(disp,GBR) src
	} else if (IS_MOVW_GBRREF_R0(code)) {	//11000101i8*2.... mov.w @(<disp>,GBR),R0
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->dst = anal_fill_ai_rg (anal, 0);
		//todo: implement @(disp,GBR) src
	} else if (IS_MOVL_GBRREF_R0(code)) {	//11000110i8*4.... mov.l @(<disp>,GBR),R0
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->dst = anal_fill_ai_rg (anal, 0);
		//todo: implement @(disp,GBR) src
	}

	return op->size;
}