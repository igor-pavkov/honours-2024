static bool fixed_msr_to_range(u32 msr, u64 *start, u64 *end)
{
	int seg, unit;

	if (!fixed_msr_to_seg_unit(msr, &seg, &unit))
		return false;

	fixed_mtrr_seg_unit_range(seg, unit, start, end);
	return true;
}