static enum dvbv3_emulation_type dvbv3_type(u32 delivery_system)
{
	switch (delivery_system) {
	case SYS_DVBC_ANNEX_A:
	case SYS_DVBC_ANNEX_C:
		return DVBV3_QAM;
	case SYS_DVBS:
	case SYS_DVBS2:
	case SYS_TURBO:
	case SYS_ISDBS:
	case SYS_DSS:
		return DVBV3_QPSK;
	case SYS_DVBT:
	case SYS_DVBT2:
	case SYS_ISDBT:
	case SYS_DTMB:
		return DVBV3_OFDM;
	case SYS_ATSC:
	case SYS_ATSCMH:
	case SYS_DVBC_ANNEX_B:
		return DVBV3_ATSC;
	case SYS_UNDEFINED:
	case SYS_ISDBC:
	case SYS_DVBH:
	case SYS_DAB:
	default:
		/*
		 * Doesn't know how to emulate those types and/or
		 * there's no frontend driver from this type yet
		 * with some emulation code, so, we're not sure yet how
		 * to handle them, or they're not compatible with a DVBv3 call.
		 */
		return DVBV3_UNKNOWN;
	}
}