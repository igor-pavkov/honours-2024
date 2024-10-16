ZEND_API int ZEND_FASTCALL zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	HT_ASSERT(&ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	if (idx != HT_INVALID_IDX) {
		while (1) {
			idx++;
			if (idx >= ht->nNumUsed) {
				*pos = HT_INVALID_IDX;
				return SUCCESS;
			}
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				*pos = idx;
				return SUCCESS;
			}
		}
	} else {
 		return FAILURE;
	}
}