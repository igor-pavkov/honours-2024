hashbin_t *hashbin_new(int type)
{
	hashbin_t* hashbin;

	/*
	 * Allocate new hashbin
	 */
	hashbin = kzalloc(sizeof(*hashbin), GFP_ATOMIC);
	if (!hashbin)
		return NULL;

	/*
	 * Initialize structure
	 */
	hashbin->hb_type = type;
	hashbin->magic = HB_MAGIC;
	//hashbin->hb_current = NULL;

	/* Make sure all spinlock's are unlocked */
	if ( hashbin->hb_type & HB_LOCK ) {
		spin_lock_init(&hashbin->hb_spinlock);
	}

	return hashbin;
}