static MngBox mng_read_box(MngBox previous_box,char delta_type,
  unsigned char *p)
{
   MngBox
      box;

  /*
    Read clipping boundaries from DEFI, CLIP, FRAM, or PAST chunk.
  */
  box.left=mng_get_long(p);
  box.right=mng_get_long(&p[4]);
  box.top=mng_get_long(&p[8]);
  box.bottom=mng_get_long(&p[12]);
  if (delta_type != 0)
    {
      box.left+=previous_box.left;
      box.right+=previous_box.right;
      box.top+=previous_box.top;
      box.bottom+=previous_box.bottom;
    }

  return(box);
}