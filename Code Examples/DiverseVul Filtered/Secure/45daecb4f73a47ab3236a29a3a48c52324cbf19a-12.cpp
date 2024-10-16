static int get_delta_q(vp8_reader *bc, int prev, int *q_update) {
  int ret_val = 0;

  if (vp8_read_bit(bc)) {
    ret_val = vp8_read_literal(bc, 4);

    if (vp8_read_bit(bc)) ret_val = -ret_val;
  }

  /* Trigger a quantizer update if the delta-q value has changed */
  if (ret_val != prev) *q_update = 1;

  return ret_val;
}