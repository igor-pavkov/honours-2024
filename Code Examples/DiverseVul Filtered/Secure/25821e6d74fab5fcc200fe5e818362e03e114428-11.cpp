static int k_comp(int e, int alpha, int gamma) {
	return ceil((alpha-e+63) * D_1_LOG2_10);
}