ipcp_vr_lattice::set_to_bottom ()
{
  if (m_vr.varying_p ())
    return false;
  /* ?? We create all sorts of VARYING ranges for floats, structures,
     and other types which we cannot handle as ranges.  We should
     probably avoid handling them throughout the pass, but it's easier
     to create a sensible VARYING here and let the lattice
     propagate.  */
  m_vr.set_varying (integer_type_node);
  return true;
}