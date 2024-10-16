static struct inotify_kernel_event * kernel_event(s32 wd, u32 mask, u32 cookie,
						  const char *name)
{
	struct inotify_kernel_event *kevent;

	kevent = kmem_cache_alloc(event_cachep, GFP_NOFS);
	if (unlikely(!kevent))
		return NULL;

	/* we hand this out to user-space, so zero it just in case */
	memset(&kevent->event, 0, sizeof(struct inotify_event));

	kevent->event.wd = wd;
	kevent->event.mask = mask;
	kevent->event.cookie = cookie;

	INIT_LIST_HEAD(&kevent->list);

	if (name) {
		size_t len, rem, event_size = sizeof(struct inotify_event);

		/*
		 * We need to pad the filename so as to properly align an
		 * array of inotify_event structures.  Because the structure is
		 * small and the common case is a small filename, we just round
		 * up to the next multiple of the structure's sizeof.  This is
		 * simple and safe for all architectures.
		 */
		len = strlen(name) + 1;
		rem = event_size - len;
		if (len > event_size) {
			rem = event_size - (len % event_size);
			if (len % event_size == 0)
				rem = 0;
		}

		kevent->name = kmalloc(len + rem, GFP_KERNEL);
		if (unlikely(!kevent->name)) {
			kmem_cache_free(event_cachep, kevent);
			return NULL;
		}
		memcpy(kevent->name, name, len);
		if (rem)
			memset(kevent->name + len, 0, rem);
		kevent->event.len = len + rem;
	} else {
		kevent->event.len = 0;
		kevent->name = NULL;
	}

	return kevent;
}