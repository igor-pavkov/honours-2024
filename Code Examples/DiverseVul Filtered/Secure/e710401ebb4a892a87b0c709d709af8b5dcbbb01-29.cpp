R_API bool r_egg_assemble_asm(REgg *egg, char **asm_list) {
	RAsmCode *asmcode = NULL;
	char *code = NULL;
	char *asm_name = NULL;

	if (asm_list) {
		char **asm_;

		for (asm_ = asm_list; *asm_; asm_ += 2) {
			if (!strcmp (egg->remit->arch, asm_[0])) {
				asm_name = asm_[1];
				break;
			}
		}
	}
	if (!asm_name) {
		if (egg->remit == &emit_x86 || egg->remit == &emit_x64) {
			asm_name = "x86.nz";
		} else if (egg->remit == &emit_arm) {
			asm_name = "arm";
		}
	}
	if (asm_name) {
		r_asm_use (egg->rasm, asm_name);
		r_asm_set_bits (egg->rasm, egg->bits);
		r_asm_set_big_endian (egg->rasm, egg->endian);
		r_asm_set_syntax (egg->rasm, R_ASM_SYNTAX_INTEL);
		code = r_buf_to_string (egg->buf);
		asmcode = r_asm_massemble (egg->rasm, code);
		if (asmcode) {
			if (asmcode->len > 0) {
				r_buf_append_bytes (egg->bin, asmcode->bytes, asmcode->len);
			}
			// LEAK r_asm_code_free (asmcode);
		} else {
			eprintf ("fail assembling\n");
		}
	}
	free (code);
	bool ret = (asmcode != NULL);
	r_asm_code_free (asmcode);
	return ret;
}