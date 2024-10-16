static ut32 const_get_value(CPU_CONST *c) {
	return c ? MASK (c->size * 8) & c->value : 0;
}