ipcp_bits_lattice::meet_with_1 (widest_int value, widest_int mask,
				unsigned precision)
{
  gcc_assert (constant_p ());

  widest_int old_mask = m_mask;
  m_mask = (m_mask | mask) | (m_value ^ value);

  if (wi::sext (m_mask, precision) == -1)
    return set_to_bottom ();

  return m_mask != old_mask;
}