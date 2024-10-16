ipcp_bits_lattice::set_to_constant (widest_int value, widest_int mask)
{
  gcc_assert (top_p ());
  m_lattice_val = IPA_BITS_CONSTANT;
  m_value = value;
  m_mask = mask;
  return true;
}