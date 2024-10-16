R_API int r_anal_bb(RAnal *anal, RAnalBlock *bb, ut64 addr, ut8 *buf, ut64 len, int head) {
	RAnalOp *op = NULL;
	int oplen, idx = 0;

	if (bb->addr == -1) {
		bb->addr = addr;
	}
	len -= 16; // XXX: hack to avoid segfault by x86im
	while (idx < len) {
		// TODO: too slow object construction
		if (!(op = r_anal_op_new ())) {
			eprintf ("Error: new (op)\n");
			return R_ANAL_RET_ERROR;
		}
		if ((oplen = r_anal_op (anal, op, addr + idx, buf + idx, len - idx, R_ANAL_OP_MASK_VAL)) == 0) {
			r_anal_op_free (op);
			op = NULL;
			if (idx == 0) {
				VERBOSE_ANAL eprintf ("Unknown opcode at 0x%08"PFMT64x"\n", addr+idx);
				return R_ANAL_RET_END;
			}
			break;
		}
		if (oplen < 1) {
			goto beach;
		}
		r_anal_bb_set_offset (bb, bb->ninstr++, addr + idx - bb->addr);
		idx += oplen;
		bb->size += oplen;
		if (head) {
			bb->type = R_ANAL_BB_TYPE_HEAD;
		}
		switch (op->type) {
		case R_ANAL_OP_TYPE_CMP:
			r_anal_cond_free (bb->cond);
			bb->cond = r_anal_cond_new_from_op (op);
			break;
		case R_ANAL_OP_TYPE_CJMP:
			if (bb->cond) {
				// TODO: get values from anal backend
				bb->cond->type = R_ANAL_COND_EQ;
			} else VERBOSE_ANAL eprintf ("Unknown conditional for block 0x%"PFMT64x"\n", bb->addr);
			bb->conditional = 1;
			bb->fail = op->fail;
			bb->jump = op->jump;
			bb->type |= R_ANAL_BB_TYPE_BODY;
			goto beach;
		case R_ANAL_OP_TYPE_JMP:
			bb->jump = op->jump;
			bb->type |= R_ANAL_BB_TYPE_BODY;
			goto beach;
		case R_ANAL_OP_TYPE_UJMP:
		case R_ANAL_OP_TYPE_IJMP:
		case R_ANAL_OP_TYPE_RJMP:
		case R_ANAL_OP_TYPE_IRJMP:
			bb->type |= R_ANAL_BB_TYPE_FOOT;
			goto beach;
		case R_ANAL_OP_TYPE_RET:
			bb->type |= R_ANAL_BB_TYPE_LAST;
			goto beach;
		case R_ANAL_OP_TYPE_LEA:
		{
			RAnalValue *src = op->src[0];
			if (src && src->reg && anal->reg) {
				const char *pc = anal->reg->name[R_REG_NAME_PC];
				RAnalValue *dst = op->dst;
				if (dst && dst->reg && !strcmp (src->reg->name, pc)) {
					int memref = anal->bits/8;
					ut8 b[8];
					ut64 ptr = idx+addr+src->delta;
					anal->iob.read_at (anal->iob.io, ptr, b, memref);
					r_anal_xrefs_set (anal, addr+idx-op->size, ptr, R_ANAL_REF_TYPE_DATA);
				}
			}
		}
		}
		r_anal_op_free (op);
	}

	return bb->size;
beach:
	r_anal_op_free (op);
	return R_ANAL_RET_END;
}