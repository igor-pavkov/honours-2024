void decoder_context::set_framerate_ratio(int percent)
{
  framerate_ratio = percent;
  calc_tid_and_framerate_ratio();
}