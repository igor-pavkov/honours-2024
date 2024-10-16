ipcp_lattice<valtype>::set_contains_variable ()
{
  bool ret = !contains_variable;
  contains_variable = true;
  return ret;
}