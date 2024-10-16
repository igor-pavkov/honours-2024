ipcp_vr_lattice::meet_with_1 (const value_range *other_vr)
{
  if (bottom_p ())
    return false;

  if (other_vr->varying_p ())
    return set_to_bottom ();

  value_range save (m_vr);
  m_vr.union_ (other_vr);
  return !m_vr.equal_p (save);
}