int generic_key_instantiate(struct key *key, struct key_preparsed_payload *prep)
{
	int ret;

	pr_devel("==>%s()\n", __func__);

	ret = key_payload_reserve(key, prep->quotalen);
	if (ret == 0) {
		rcu_assign_keypointer(key, prep->payload.data[0]);
		key->payload.data[1] = prep->payload.data[1];
		key->payload.data[2] = prep->payload.data[2];
		key->payload.data[3] = prep->payload.data[3];
		prep->payload.data[0] = NULL;
		prep->payload.data[1] = NULL;
		prep->payload.data[2] = NULL;
		prep->payload.data[3] = NULL;
	}
	pr_devel("<==%s() = %d\n", __func__, ret);
	return ret;
}