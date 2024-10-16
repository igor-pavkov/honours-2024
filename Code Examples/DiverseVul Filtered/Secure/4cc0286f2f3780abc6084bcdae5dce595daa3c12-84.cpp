ZEND_API void ZEND_FASTCALL zend_hash_destroy(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) <= 1);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->pDestructor) {
			SET_INCONSISTENT(HT_IS_DESTROYING);

			if (ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS)) {
				if (ht->nNumUsed == ht->nNumOfElements) {
					do {
						ht->pDestructor(&p->val);
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							ht->pDestructor(&p->val);
						}
					} while (++p != end);
				}
			} else if (ht->nNumUsed == ht->nNumOfElements) {
				do {
					ht->pDestructor(&p->val);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				} while (++p != end);
			} else {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}

			SET_INCONSISTENT(HT_DESTROYED);
		} else {
			if (!(ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS))) {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		}
		zend_hash_iterators_remove(ht);
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		return;
	}
	pefree(HT_GET_DATA_ADDR(ht), ht->u.flags & HASH_FLAG_PERSISTENT);
}