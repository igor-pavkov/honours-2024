static pyc_object *get_code_object(RzBuffer *buffer) {
	bool error = false;

	pyc_object *ret = RZ_NEW0(pyc_object);
	pyc_code_object *cobj = RZ_NEW0(pyc_code_object);
	if (!ret || !cobj) {
		free(ret);
		free(cobj);
		return NULL;
	}

	// ret->type = TYPE_CODE_v1;
	//  support start from v1.0
	ret->data = cobj;

	bool v10_to_12 = magic_int_within(magic_int, 39170, 16679, &error); // 1.0.1 - 1.2
	bool v13_to_22 = magic_int_within(magic_int, 11913, 60718, &error); // 1.3b1 - 2.2a1
	bool v11_to_14 = magic_int_within(magic_int, 39170, 20117, &error); // 1.0.1 - 1.4
	bool v15_to_22 = magic_int_within(magic_int, 20121, 60718, &error); // 1.5a1 - 2.2a1
	bool v13_to_20 = magic_int_within(magic_int, 11913, 50824, &error); // 1.3b1 - 2.0b1
	// bool v21_to_27 = (!v13_to_20) && magic_int_within (magic_int, 60124, 62212, &error);
	bool has_posonlyargcount = magic_int_within(magic_int, 3410, 3424, &error); // v3.8.0a4 - latest
	if (error) {
		free(ret);
		free(cobj);
		return NULL;
	}

	if (v13_to_22) {
		cobj->argcount = get_ut16(buffer, &error);
	} else if (v10_to_12) {
		cobj->argcount = 0;
	} else {
		cobj->argcount = get_ut32(buffer, &error);
	}

	if (has_posonlyargcount) {
		cobj->posonlyargcount = get_ut32(buffer, &error); // Included in argcount
	} else {
		cobj->posonlyargcount = 0; // None
	}

	if (((3020 < (magic_int & 0xffff)) && ((magic_int & 0xffff) < 20121)) && (!v11_to_14)) {
		cobj->kwonlyargcount = get_ut32(buffer, &error); // Not included in argcount
	} else {
		cobj->kwonlyargcount = 0;
	}

	if (v13_to_22) {
		cobj->nlocals = get_ut16(buffer, &error);
	} else if (v10_to_12) {
		cobj->nlocals = 0;
	} else {
		cobj->nlocals = get_ut32(buffer, &error);
	}

	if (v15_to_22) {
		cobj->stacksize = get_ut16(buffer, &error);
	} else if (v11_to_14 || v10_to_12) {
		cobj->stacksize = 0;
	} else {
		cobj->stacksize = get_ut32(buffer, &error);
	}

	if (v13_to_22) {
		cobj->flags = get_ut16(buffer, &error);
	} else if (v10_to_12) {
		cobj->flags = 0;
	} else {
		cobj->flags = get_ut32(buffer, &error);
	}

	// to help disassemble the code
	cobj->start_offset = rz_buf_tell(buffer) + 5; // 1 from get_object() and 4 from get_string_object()
	if (!refs) {
		return ret; // return for entried part to get the root object of this file
	}
	cobj->code = get_object(buffer);
	cobj->end_offset = rz_buf_tell(buffer);

	cobj->consts = get_object(buffer);
	cobj->names = get_object(buffer);

	if (v10_to_12) {
		cobj->varnames = NULL;
	} else {
		cobj->varnames = get_object(buffer);
	}

	if (!(v10_to_12 || v13_to_20)) {
		cobj->freevars = get_object(buffer);
		cobj->cellvars = get_object(buffer);
	} else {
		cobj->freevars = NULL;
		cobj->cellvars = NULL;
	}

	cobj->filename = get_object(buffer);
	cobj->name = get_object(buffer);

	if (v15_to_22) {
		cobj->firstlineno = get_ut16(buffer, &error);
	} else if (v11_to_14) {
		cobj->firstlineno = 0;
	} else {
		cobj->firstlineno = get_ut32(buffer, &error);
	}

	if (v11_to_14) {
		cobj->lnotab = NULL;
	} else {
		cobj->lnotab = get_object(buffer);
	}

	if (error) {
		free_object(cobj->code);
		free_object(cobj->consts);
		free_object(cobj->names);
		free_object(cobj->varnames);
		free_object(cobj->freevars);
		free_object(cobj->cellvars);
		free_object(cobj->filename);
		free_object(cobj->name);
		free_object(cobj->lnotab);
		free(cobj);
		RZ_FREE(ret);
		return NULL;
	}
	return ret;
}