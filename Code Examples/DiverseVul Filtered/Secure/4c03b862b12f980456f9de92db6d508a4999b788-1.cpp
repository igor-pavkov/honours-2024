void hashbin_insert(hashbin_t* hashbin, irda_queue_t* entry, long hashv,
		    const char* name)
{
	unsigned long flags = 0;
	int bin;

	IRDA_ASSERT( hashbin != NULL, return;);
	IRDA_ASSERT( hashbin->magic == HB_MAGIC, return;);

	/*
	 * Locate hashbin
	 */
	if ( name )
		hashv = hash( name );
	bin = GET_HASHBIN( hashv );

	/* Synchronize */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_lock_irqsave(&hashbin->hb_spinlock, flags);
	} /* Default is no-lock  */

	/*
	 * Store name and key
	 */
	entry->q_hash = hashv;
	if ( name )
		strlcpy( entry->q_name, name, sizeof(entry->q_name));

	/*
	 * Insert new entry first
	 */
	enqueue_first( (irda_queue_t**) &hashbin->hb_queue[ bin ],
		       entry);
	hashbin->hb_size++;

	/* Release lock */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_unlock_irqrestore(&hashbin->hb_spinlock, flags);
	} /* Default is no-lock  */
}