static int tipc_nl_compat_link_stat_dump(struct tipc_nl_compat_msg *msg,
					 struct nlattr **attrs)
{
	char *name;
	struct nlattr *link[TIPC_NLA_LINK_MAX + 1];
	struct nlattr *prop[TIPC_NLA_PROP_MAX + 1];
	struct nlattr *stats[TIPC_NLA_STATS_MAX + 1];
	int err;

	if (!attrs[TIPC_NLA_LINK])
		return -EINVAL;

	err = nla_parse_nested(link, TIPC_NLA_LINK_MAX, attrs[TIPC_NLA_LINK],
			       NULL);
	if (err)
		return err;

	if (!link[TIPC_NLA_LINK_PROP])
		return -EINVAL;

	err = nla_parse_nested(prop, TIPC_NLA_PROP_MAX,
			       link[TIPC_NLA_LINK_PROP], NULL);
	if (err)
		return err;

	if (!link[TIPC_NLA_LINK_STATS])
		return -EINVAL;

	err = nla_parse_nested(stats, TIPC_NLA_STATS_MAX,
			       link[TIPC_NLA_LINK_STATS], NULL);
	if (err)
		return err;

	name = (char *)TLV_DATA(msg->req);
	if (strcmp(name, nla_data(link[TIPC_NLA_LINK_NAME])) != 0)
		return 0;

	tipc_tlv_sprintf(msg->rep, "\nLink <%s>\n",
			 nla_data(link[TIPC_NLA_LINK_NAME]));

	if (link[TIPC_NLA_LINK_BROADCAST]) {
		__fill_bc_link_stat(msg, prop, stats);
		return 0;
	}

	if (link[TIPC_NLA_LINK_ACTIVE])
		tipc_tlv_sprintf(msg->rep, "  ACTIVE");
	else if (link[TIPC_NLA_LINK_UP])
		tipc_tlv_sprintf(msg->rep, "  STANDBY");
	else
		tipc_tlv_sprintf(msg->rep, "  DEFUNCT");

	tipc_tlv_sprintf(msg->rep, "  MTU:%u  Priority:%u",
			 nla_get_u32(link[TIPC_NLA_LINK_MTU]),
			 nla_get_u32(prop[TIPC_NLA_PROP_PRIO]));

	tipc_tlv_sprintf(msg->rep, "  Tolerance:%u ms  Window:%u packets\n",
			 nla_get_u32(prop[TIPC_NLA_PROP_TOL]),
			 nla_get_u32(prop[TIPC_NLA_PROP_WIN]));

	tipc_tlv_sprintf(msg->rep,
			 "  RX packets:%u fragments:%u/%u bundles:%u/%u\n",
			 nla_get_u32(link[TIPC_NLA_LINK_RX]) -
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_INFO]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_FRAGMENTS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_FRAGMENTED]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_BUNDLES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_BUNDLED]));

	tipc_tlv_sprintf(msg->rep,
			 "  TX packets:%u fragments:%u/%u bundles:%u/%u\n",
			 nla_get_u32(link[TIPC_NLA_LINK_TX]) -
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_INFO]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_FRAGMENTS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_FRAGMENTED]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_BUNDLES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_BUNDLED]));

	tipc_tlv_sprintf(msg->rep,
			 "  TX profile sample:%u packets  average:%u octets\n",
			 nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_CNT]),
			 nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_TOT]) /
			 nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT]));

	tipc_tlv_sprintf(msg->rep,
			 "  0-64:%u%% -256:%u%% -1024:%u%% -4096:%u%% ",
			 perc(nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_P0]),
			      nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT])),
			 perc(nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_P1]),
			      nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT])),
			 perc(nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_P2]),
			      nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT])),
			 perc(nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_P3]),
			      nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT])));

	tipc_tlv_sprintf(msg->rep, "-16384:%u%% -32768:%u%% -66000:%u%%\n",
			 perc(nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_P4]),
			      nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT])),
			 perc(nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_P5]),
			      nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT])),
			 perc(nla_get_u32(stats[TIPC_NLA_STATS_MSG_LEN_P6]),
			      nla_get_u32(stats[TIPC_NLA_STATS_MSG_PROF_TOT])));

	tipc_tlv_sprintf(msg->rep,
			 "  RX states:%u probes:%u naks:%u defs:%u dups:%u\n",
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_STATES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_PROBES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_NACKS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_DEFERRED]),
			 nla_get_u32(stats[TIPC_NLA_STATS_DUPLICATES]));

	tipc_tlv_sprintf(msg->rep,
			 "  TX states:%u probes:%u naks:%u acks:%u dups:%u\n",
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_STATES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_PROBES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_NACKS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_ACKS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RETRANSMITTED]));

	tipc_tlv_sprintf(msg->rep,
			 "  Congestion link:%u  Send queue max:%u avg:%u",
			 nla_get_u32(stats[TIPC_NLA_STATS_LINK_CONGS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_MAX_QUEUE]),
			 nla_get_u32(stats[TIPC_NLA_STATS_AVG_QUEUE]));

	return 0;
}