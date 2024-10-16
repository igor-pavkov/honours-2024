void* hashbin_find_next( hashbin_t* hashbin, long hashv, const char* name,
			 void ** pnext)
{
	unsigned long flags = 0;
	irda_queue_t* entry;

	/* Synchronize */
	spin_lock_irqsave(&hashbin->hb_spinlock, flags);

	/*
	 * Search for current entry
	 * This allow to check if the current item is still in the
	 * hashbin or has been removed.
	 */
	entry = hashbin_find(hashbin, hashv, name);

	/*
	 * Trick hashbin_get_next() to return what we want
	 */
	if(entry) {
		hashbin->hb_current = entry;
		*pnext = hashbin_get_next( hashbin );
	} else
		*pnext = NULL;

	/* Release lock */
	spin_unlock_irqrestore(&hashbin->hb_spinlock, flags);

	return entry;
}