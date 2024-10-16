PHPAPI int php_var_unserialize_ex(UNSERIALIZE_PARAMETER)
{
	var_entries *orig_var_entries = (*var_hash)->last;
	zend_long orig_used_slots = orig_var_entries ? orig_var_entries->used_slots : 0;
	int result;
	
	result = php_var_unserialize_internal(UNSERIALIZE_PASSTHRU);

	if (!result) {
		/* If the unserialization failed, mark all elements that have been added to var_hash
		 * as NULL. This will forbid their use by other unserialize() calls in the same
		 * unserialization context. */
		var_entries *e = orig_var_entries;
		zend_long s = orig_used_slots;
		while (e) {
			for (; s < e->used_slots; s++) {
				e->data[s] = NULL;
			}

			e = e->next;
			s = 0;
		}
	}

	return result;
}