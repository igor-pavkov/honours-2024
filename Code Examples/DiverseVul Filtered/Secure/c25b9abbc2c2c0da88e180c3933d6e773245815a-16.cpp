void rmt_state_change(struct s_smc *smc, int r_state)
{
#ifdef DRIVERDEBUG
	char *s;

	switch (r_state) {
	case RM0_ISOLATED:
		s = "RM0_ISOLATED";
		break;
	case RM1_NON_OP:
		s = "RM1_NON_OP - not operational";
		break;
	case RM2_RING_OP:
		s = "RM2_RING_OP - ring operational";
		break;
	case RM3_DETECT:
		s = "RM3_DETECT - detect dupl addresses";
		break;
	case RM4_NON_OP_DUP:
		s = "RM4_NON_OP_DUP - dupl. addr detected";
		break;
	case RM5_RING_OP_DUP:
		s = "RM5_RING_OP_DUP - ring oper. with dupl. addr";
		break;
	case RM6_DIRECTED:
		s = "RM6_DIRECTED - sending directed beacons";
		break;
	case RM7_TRACE:
		s = "RM7_TRACE - trace initiated";
		break;
	default:
		s = "unknown";
		break;
	}
	PRINTK(KERN_INFO "[rmt_state_change: %s]\n", s);
#endif				// DRIVERDEBUG
}				// rmt_state_change