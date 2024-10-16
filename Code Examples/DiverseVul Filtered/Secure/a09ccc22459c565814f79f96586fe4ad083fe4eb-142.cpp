ipcp_lattice<valtype>::is_single_const ()
{
  if (bottom || contains_variable || values_count != 1)
    return false;
  else
    return true;
}