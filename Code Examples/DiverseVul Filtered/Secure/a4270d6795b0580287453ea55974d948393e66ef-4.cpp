int dev_set_alias(struct net_device *dev, const char *alias, size_t len)
{
	struct dev_ifalias *new_alias = NULL;

	if (len >= IFALIASZ)
		return -EINVAL;

	if (len) {
		new_alias = kmalloc(sizeof(*new_alias) + len + 1, GFP_KERNEL);
		if (!new_alias)
			return -ENOMEM;

		memcpy(new_alias->ifalias, alias, len);
		new_alias->ifalias[len] = 0;
	}

	mutex_lock(&ifalias_mutex);
	rcu_swap_protected(dev->ifalias, new_alias,
			   mutex_is_locked(&ifalias_mutex));
	mutex_unlock(&ifalias_mutex);

	if (new_alias)
		kfree_rcu(new_alias, rcuhead);

	return len;
}