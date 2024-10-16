static int first_nibble_is_8(RAnal* anal, RAnalOp* op, ut16 code){
	if (IS_BT_OR_BF(code)) {
		op->type = R_ANAL_OP_TYPE_CJMP; //Jump if true or jump if false insns
		op->jump = disarm_8bit_offset (op->addr, GET_BTF_OFFSET(code));
		op->fail = op->addr + 2 ;
		op->eob  = true;
		if (IS_BTS(code) || IS_BFS(code))
			op->delay = 1; //Only /S versions have a delay slot
	} else if (IS_MOVB_REGDISP_R0(code)) {
		// 10000100mmmmi4*1 mov.b @(<disp>,<REG_M>),R0
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->dst = anal_fill_ai_rg (anal, 0);
		op->src[0] = anal_fill_reg_disp_mem (anal, GET_SOURCE_REG(code), code&0x0F, BYTE_SIZE);
	} else if (IS_MOVW_REGDISP_R0(code)) {
		// 10000101mmmmi4*2 mov.w @(<disp>,<REG_M>),R0
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->dst = anal_fill_ai_rg (anal, 0);
		op->src[0] = anal_fill_reg_disp_mem (anal, GET_SOURCE_REG(code), code&0x0F, WORD_SIZE);
	} else if (IS_CMPIMM(code)) {
		op->type = R_ANAL_OP_TYPE_CMP;
		//todo : finish implementing
	} else if (IS_MOVB_R0_REGDISP(code)) {
		/* 10000000mmmmi4*1 mov.b R0,@(<disp>,<REG_M>)*/
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, 0);
		op->dst = anal_fill_reg_disp_mem (anal, GET_SOURCE_REG(code), code&0x0F, BYTE_SIZE);
	} else if (IS_MOVW_R0_REGDISP(code)) {
		// 10000001mmmmi4*2 mov.w R0,@(<disp>,<REG_M>))
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, 0);
		op->dst = anal_fill_reg_disp_mem (anal, GET_SOURCE_REG(code), code&0x0F, WORD_SIZE);
	}
	return op->size;
}