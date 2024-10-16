void ecm_state_change(struct s_smc *smc, int e_state)
{
#ifdef DRIVERDEBUG
	char *s;

	switch (e_state) {
	case EC0_OUT:
		s = "EC0_OUT";
		break;
	case EC1_IN:
		s = "EC1_IN";
		break;
	case EC2_TRACE:
		s = "EC2_TRACE";
		break;
	case EC3_LEAVE:
		s = "EC3_LEAVE";
		break;
	case EC4_PATH_TEST:
		s = "EC4_PATH_TEST";
		break;
	case EC5_INSERT:
		s = "EC5_INSERT";
		break;
	case EC6_CHECK:
		s = "EC6_CHECK";
		break;
	case EC7_DEINSERT:
		s = "EC7_DEINSERT";
		break;
	default:
		s = "unknown";
		break;
	}
	PRINTK(KERN_INFO "ecm_state_change: %s\n", s);
#endif				//DRIVERDEBUG
}				// ecm_state_change