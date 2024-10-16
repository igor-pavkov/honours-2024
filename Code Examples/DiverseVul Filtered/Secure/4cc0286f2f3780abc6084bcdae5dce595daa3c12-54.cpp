ZEND_API void ZEND_FASTCALL zend_symtable_clean(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->u.flags & HASH_FLAG_STATIC_KEYS) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
			} while (++p != end);
		} else if (ht->nNumUsed == ht->nNumOfElements) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
				if (EXPECTED(p->key)) {
					zend_string_release(p->key);
				}
			} while (++p != end);
		} else {
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				}
			} while (++p != end);
		}
		HT_HASH_RESET(ht);
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = HT_INVALID_IDX;
}