static void __fill_bc_link_stat(struct tipc_nl_compat_msg *msg,
				struct nlattr *prop[], struct nlattr *stats[])
{
	tipc_tlv_sprintf(msg->rep, "  Window:%u packets\n",
			 nla_get_u32(prop[TIPC_NLA_PROP_WIN]));

	tipc_tlv_sprintf(msg->rep,
			 "  RX packets:%u fragments:%u/%u bundles:%u/%u\n",
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_INFO]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_FRAGMENTS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_FRAGMENTED]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_BUNDLES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_BUNDLED]));

	tipc_tlv_sprintf(msg->rep,
			 "  TX packets:%u fragments:%u/%u bundles:%u/%u\n",
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_INFO]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_FRAGMENTS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_FRAGMENTED]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_BUNDLES]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_BUNDLED]));

	tipc_tlv_sprintf(msg->rep, "  RX naks:%u defs:%u dups:%u\n",
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_NACKS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RX_DEFERRED]),
			 nla_get_u32(stats[TIPC_NLA_STATS_DUPLICATES]));

	tipc_tlv_sprintf(msg->rep, "  TX naks:%u acks:%u dups:%u\n",
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_NACKS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_TX_ACKS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_RETRANSMITTED]));

	tipc_tlv_sprintf(msg->rep,
			 "  Congestion link:%u  Send queue max:%u avg:%u",
			 nla_get_u32(stats[TIPC_NLA_STATS_LINK_CONGS]),
			 nla_get_u32(stats[TIPC_NLA_STATS_MAX_QUEUE]),
			 nla_get_u32(stats[TIPC_NLA_STATS_AVG_QUEUE]));
}