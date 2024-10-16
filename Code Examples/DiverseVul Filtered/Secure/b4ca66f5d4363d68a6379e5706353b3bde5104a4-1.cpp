static inline RBinWasmTypeVec *parse_type_vector(RBuffer *b, ut64 bound) {
	RBinWasmTypeVec *vec = R_NEW0 (RBinWasmTypeVec);
	// types are all ut8, so leb128 shouldn't be needed, we can reuse consume_str_new
	if (vec && !consume_str_new (b, bound, &vec->count, (char **)&vec->types)) {
		free_type_vec (vec);
		return NULL;
	}
	return vec;
}