static bool setsockopt_needs_rtnl(int optname)
{
	switch (optname) {
	case IPV6_ADDRFORM:
	case IPV6_ADD_MEMBERSHIP:
	case IPV6_DROP_MEMBERSHIP:
	case IPV6_JOIN_ANYCAST:
	case IPV6_LEAVE_ANYCAST:
	case MCAST_JOIN_GROUP:
	case MCAST_LEAVE_GROUP:
	case MCAST_JOIN_SOURCE_GROUP:
	case MCAST_LEAVE_SOURCE_GROUP:
	case MCAST_BLOCK_SOURCE:
	case MCAST_UNBLOCK_SOURCE:
	case MCAST_MSFILTER:
		return true;
	}
	return false;
}