static bool skip_this_pass(int severity)
{
    /*
     * See if it's a pass-specific error or warning which should be skipped.
     * We cannot skip errors stronger than ERR_NONFATAL as by definition
     * they cannot be resumed from.
     */
    if ((severity & ERR_MASK) > ERR_NONFATAL)
	return false;

    /*
     * passn is 1 on the very first pass only.
     * pass0 is 2 on the code-generation (final) pass only.
     * These are the passes we care about in this case.
     */
    return (((severity & ERR_PASS1) && passn != 1) ||
	    ((severity & ERR_PASS2) && pass0 != 2));
}