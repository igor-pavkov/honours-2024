static int regvar_comparator(const RAnalVar *a, const RAnalVar *b){
	// avoid NULL dereference
	return (a && b)? (a->argnum > b->argnum) - (a->argnum < b->argnum): 0;
}