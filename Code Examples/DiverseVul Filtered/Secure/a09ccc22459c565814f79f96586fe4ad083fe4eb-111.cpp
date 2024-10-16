ipcp_bits_lattice::get_value_and_mask (tree operand, widest_int *valuep, widest_int *maskp)
{
  wide_int get_nonzero_bits (const_tree);

  if (TREE_CODE (operand) == INTEGER_CST)
    {
      *valuep = wi::to_widest (operand);
      *maskp = 0;
    }
  else
    {
      *valuep = 0;
      *maskp = -1;
    }
}