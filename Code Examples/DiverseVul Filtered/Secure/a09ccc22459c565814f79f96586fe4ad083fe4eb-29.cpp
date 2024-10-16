ipcp_bits_lattice::set_to_bottom ()
{
  if (bottom_p ())
    return false;
  m_lattice_val = IPA_BITS_VARYING;
  m_value = 0;
  m_mask = -1;
  return true;
}