static void stbi__out_gif_code(stbi__gif *g, stbi__uint16 code)
{
   stbi_uc *p, *c;
   int idx;

   // recurse to decode the prefixes, since the linked-list is backwards,
   // and working backwards through an interleaved image would be nasty
   if (g->codes[code].prefix >= 0)
      stbi__out_gif_code(g, g->codes[code].prefix);

   if (g->cur_y >= g->max_y) return;

   idx = g->cur_x + g->cur_y;
   p = &g->out[idx];
   g->history[idx / 4] = 1;

   c = &g->color_table[g->codes[code].suffix * 4];
   if (c[3] > 128) { // don't render transparent pixels;
      p[0] = c[2];
      p[1] = c[1];
      p[2] = c[0];
      p[3] = c[3];
   }
   g->cur_x += 4;

   if (g->cur_x >= g->max_x) {
      g->cur_x = g->start_x;
      g->cur_y += g->step;

      while (g->cur_y >= g->max_y && g->parse > 0) {
         g->step = (1 << g->parse) * g->line_size;
         g->cur_y = g->start_y + (g->step >> 1);
         --g->parse;
      }
   }
}