ipcp_bits_lattice::meet_with (ipcp_bits_lattice& other, unsigned precision,
			      signop sgn, enum tree_code code, tree operand)
{
  if (other.bottom_p ())
    return set_to_bottom ();

  if (bottom_p () || other.top_p ())
    return false;

  widest_int adjusted_value, adjusted_mask;

  if (TREE_CODE_CLASS (code) == tcc_binary)
    {
      tree type = TREE_TYPE (operand);
      widest_int o_value, o_mask;
      get_value_and_mask (operand, &o_value, &o_mask);

      bit_value_binop (code, sgn, precision, &adjusted_value, &adjusted_mask,
		       sgn, precision, other.get_value (), other.get_mask (),
		       TYPE_SIGN (type), TYPE_PRECISION (type), o_value, o_mask);

      if (wi::sext (adjusted_mask, precision) == -1)
	return set_to_bottom ();
    }

  else if (TREE_CODE_CLASS (code) == tcc_unary)
    {
      bit_value_unop (code, sgn, precision, &adjusted_value,
		      &adjusted_mask, sgn, precision, other.get_value (),
		      other.get_mask ());

      if (wi::sext (adjusted_mask, precision) == -1)
	return set_to_bottom ();
    }

  else
    return set_to_bottom ();

  if (top_p ())
    {
      if (wi::sext (adjusted_mask, precision) == -1)
	return set_to_bottom ();
      return set_to_constant (adjusted_value, adjusted_mask);
    }
  else
    return meet_with_1 (adjusted_value, adjusted_mask, precision);
}