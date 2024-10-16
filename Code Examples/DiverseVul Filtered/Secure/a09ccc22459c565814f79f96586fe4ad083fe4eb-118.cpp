ipcp_bits_lattice::print (FILE *f)
{
  if (top_p ())
    fprintf (f, "         Bits unknown (TOP)\n");
  else if (bottom_p ())
    fprintf (f, "         Bits unusable (BOTTOM)\n");
  else
    {
      fprintf (f, "         Bits: value = "); print_hex (get_value (), f);
      fprintf (f, ", mask = "); print_hex (get_mask (), f);
      fprintf (f, "\n");
    }
}