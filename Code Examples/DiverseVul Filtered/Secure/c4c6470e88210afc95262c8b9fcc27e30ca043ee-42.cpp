static int get_accel(const blosc2_context* context) {
  int clevel = context->clevel;

  if (context->compcode == BLOSC_LZ4) {
    /* This acceleration setting based on discussions held in:
     * https://groups.google.com/forum/#!topic/lz4c/zosy90P8MQw
     */
    return (10 - clevel);
  }
  else if (context->compcode == BLOSC_LIZARD) {
    /* Lizard currently accepts clevels from 10 to 49 */
      switch (clevel) {
        case 1 :
            return 10;
        case 2 :
            return 10;
        case 3 :
            return 10;
        case 4 :
            return 10;
        case 5 :
            return 20;
        case 6 :
            return 20;
        case 7 :
            return 20;
        case 8 :
            return 41;
        case 9 :
            return 41;
        default :
          break;
      }
  }
  return 1;
}