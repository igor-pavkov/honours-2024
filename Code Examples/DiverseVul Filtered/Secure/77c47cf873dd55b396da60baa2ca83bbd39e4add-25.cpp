static int first_nibble_is_4(RAnal* anal, RAnalOp* op, ut16 code){
	switch (code & 0xF0FF) {
		//todo: implement
	case 0x4020:	//shal
		op->type = R_ANAL_OP_TYPE_SAL;
		break;
	case 0x4021:	//shar
		op->type = R_ANAL_OP_TYPE_SAR;
		break;
	case 0x4000:	//shll
	case 0x4008:	//shll2
	case 0x4018:	//shll8
	case 0x4028:	//shll16
		op->type = R_ANAL_OP_TYPE_SHL;
		break;
	case 0x4001:	//shlr
	case 0x4009:	//shlr2
	case 0x4019:	//shlr8
	case 0x4029:	//shlr16
		op->type = R_ANAL_OP_TYPE_SHR;
		break;
	default:
		break;
	}

	if (IS_JSR(code)) {
		op->type = R_ANAL_OP_TYPE_UCALL; //call to reg
		op->delay = 1;
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if ( IS_JMP(code) ) {
		op->type = R_ANAL_OP_TYPE_UJMP; //jmp to reg
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		op->delay = 1;
		op->eob = true;
	} else if (IS_CMPPL(code) || IS_CMPPZ(code)) {
		op->type = R_ANAL_OP_TYPE_CMP;
		//todo: implement
	} else if (IS_LDCLSR1(code) || IS_LDSLMAC(code) || IS_LDSLPR(code)) {
		op->type = R_ANAL_OP_TYPE_POP;
		//todo: implement
	} else if (IS_LDCSR1(code) || IS_LDSMAC(code) || IS_LDSPR(code)) {
		op->type = R_ANAL_OP_TYPE_MOV;
		//todo: implement
	} else if (IS_ROT(code)) {
		op->type = (code&1)? R_ANAL_OP_TYPE_ROR:R_ANAL_OP_TYPE_ROL;
		//todo: implement rot* vs rotc*
	} else if (IS_STCLSR1(code) || IS_STSLMAC(code) || IS_STSLPR(code)) {
		op->type = R_ANAL_OP_TYPE_PUSH;
		//todo: implement st*.l *,@-Rn
	} else if (IS_TASB(code)) {
		op->type = R_ANAL_OP_TYPE_UNK;
		//todo: implement
	} else if (IS_DT(code)) {
		op->type = R_ANAL_OP_TYPE_UNK;
		//todo: implement
	}
	return op->size;
}