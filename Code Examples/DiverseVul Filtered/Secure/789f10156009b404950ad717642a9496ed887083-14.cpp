uint32_t intsetLen(const intset *is) {
    return intrev32ifbe(is->length);
}