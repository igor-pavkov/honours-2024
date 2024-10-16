static int oprep(RAsm *a, ut8 *data, const Opcode *op) {
	int l = 0;
	LookupTable *lt_ptr;
	int retval;
	if (!strcmp (op->mnemonic, "rep") ||
	    !strcmp (op->mnemonic, "repe") ||
	    !strcmp (op->mnemonic, "repz")) {
		data[l++] = 0xf3;
	} else if (!strcmp (op->mnemonic, "repne") ||
	           !strcmp (op->mnemonic, "repnz")) {
		data[l++] = 0xf2;
	}
	Opcode instr = {0};
	parseOpcode (a, op->operands[0].rep_op, &instr);

	for (lt_ptr = oplookup; strcmp (lt_ptr->mnemonic, "null"); lt_ptr++) {
		if (!r_str_casecmp (instr.mnemonic, lt_ptr->mnemonic)) {
			if (lt_ptr->opcode > 0) {
				if (lt_ptr->only_x32 && a->bits == 64) {
					return -1;
				}
				ut8 *ptr = (ut8 *)&lt_ptr->opcode;
				int i = 0;
				for (; i < lt_ptr->size; i++) {
					data[i + l] = ptr[lt_ptr->size - (i + 1)];
				}
				free (instr.mnemonic);
				return l + lt_ptr->size;
			} else {
				if (lt_ptr->opdo) {
					data += l;
					if (instr.has_bnd) {
						data[l] = 0xf2;
						data++;
					}
					retval = lt_ptr->opdo (a, data, &instr);
					// if op supports bnd then the first byte will
					// be 0xf2.
					if (instr.has_bnd) {
						retval++;
					}
					return l + retval;
				}
				break;
			}
		}
	}
	free (instr.mnemonic);
	return -1;
}