ipcp_bits_lattice::meet_with (widest_int value, widest_int mask,
			      unsigned precision)
{
  if (bottom_p ())
    return false;

  if (top_p ())
    {
      if (wi::sext (mask, precision) == -1)
	return set_to_bottom ();
      return set_to_constant (value, mask);
    }

  return meet_with_1 (value, mask, precision);
}