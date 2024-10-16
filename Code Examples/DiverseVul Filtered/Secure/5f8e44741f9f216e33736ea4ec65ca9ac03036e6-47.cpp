int rtnetlink_put_metrics(struct sk_buff *skb, u32 *metrics)
{
	struct nlattr *mx;
	int i, valid = 0;

	mx = nla_nest_start(skb, RTA_METRICS);
	if (mx == NULL)
		return -ENOBUFS;

	for (i = 0; i < RTAX_MAX; i++) {
		if (metrics[i]) {
			if (i == RTAX_CC_ALGO - 1) {
				char tmp[TCP_CA_NAME_MAX], *name;

				name = tcp_ca_get_name_by_key(metrics[i], tmp);
				if (!name)
					continue;
				if (nla_put_string(skb, i + 1, name))
					goto nla_put_failure;
			} else if (i == RTAX_FEATURES - 1) {
				u32 user_features = metrics[i] & RTAX_FEATURE_MASK;

				BUILD_BUG_ON(RTAX_FEATURE_MASK & DST_FEATURE_MASK);
				if (nla_put_u32(skb, i + 1, user_features))
					goto nla_put_failure;
			} else {
				if (nla_put_u32(skb, i + 1, metrics[i]))
					goto nla_put_failure;
			}
			valid++;
		}
	}

	if (!valid) {
		nla_nest_cancel(skb, mx);
		return 0;
	}

	return nla_nest_end(skb, mx);

nla_put_failure:
	nla_nest_cancel(skb, mx);
	return -EMSGSIZE;
}