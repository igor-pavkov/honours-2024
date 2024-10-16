static int flow_to_policy_dir(int dir)
{
	if (XFRM_POLICY_IN == FLOW_DIR_IN &&
	    XFRM_POLICY_OUT == FLOW_DIR_OUT &&
	    XFRM_POLICY_FWD == FLOW_DIR_FWD)
		return dir;

	switch (dir) {
	default:
	case FLOW_DIR_IN:
		return XFRM_POLICY_IN;
	case FLOW_DIR_OUT:
		return XFRM_POLICY_OUT;
	case FLOW_DIR_FWD:
		return XFRM_POLICY_FWD;
	}
}