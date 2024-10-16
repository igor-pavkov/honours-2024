ipcp_lattice<valtype>::set_to_bottom ()
{
  bool ret = !bottom;
  bottom = true;
  return ret;
}