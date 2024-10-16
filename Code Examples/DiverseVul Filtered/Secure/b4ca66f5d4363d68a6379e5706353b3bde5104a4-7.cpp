static void free_type_vec(RBinWasmTypeVec *vec) {
	if (vec) {
		free (vec->types);
		free (vec);
	}
}