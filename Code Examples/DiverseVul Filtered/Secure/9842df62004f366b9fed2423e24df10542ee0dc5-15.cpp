static bool fixed_msr_to_seg_unit(u32 msr, int *seg, int *unit)
{
	switch (msr) {
	case MSR_MTRRfix64K_00000:
		*seg = 0;
		*unit = 0;
		break;
	case MSR_MTRRfix16K_80000 ... MSR_MTRRfix16K_A0000:
		*seg = 1;
		*unit = msr - MSR_MTRRfix16K_80000;
		break;
	case MSR_MTRRfix4K_C0000 ... MSR_MTRRfix4K_F8000:
		*seg = 2;
		*unit = msr - MSR_MTRRfix4K_C0000;
		break;
	default:
		return false;
	}

	return true;
}