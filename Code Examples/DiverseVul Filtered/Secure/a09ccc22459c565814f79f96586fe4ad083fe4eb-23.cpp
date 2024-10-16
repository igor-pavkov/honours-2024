ipcp_vr_lattice::meet_with (const ipcp_vr_lattice &other)
{
  return meet_with_1 (&other.m_vr);
}