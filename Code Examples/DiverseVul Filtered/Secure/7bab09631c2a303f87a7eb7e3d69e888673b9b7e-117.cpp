static inline int policy_to_flow_dir(int dir)
{
	if (XFRM_POLICY_IN == FLOW_DIR_IN &&
	    XFRM_POLICY_OUT == FLOW_DIR_OUT &&
	    XFRM_POLICY_FWD == FLOW_DIR_FWD)
		return dir;
	switch (dir) {
	default:
	case XFRM_POLICY_IN:
		return FLOW_DIR_IN;
	case XFRM_POLICY_OUT:
		return FLOW_DIR_OUT;
	case XFRM_POLICY_FWD:
		return FLOW_DIR_FWD;
	}
}