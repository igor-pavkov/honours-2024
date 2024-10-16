thread_context::thread_context()
{
  /*
  CtbAddrInRS = 0;
  CtbAddrInTS = 0;

  CtbX = 0;
  CtbY = 0;
  */

  /*
  refIdx[0] = refIdx[1] = 0;
  mvd[0][0] = mvd[0][1] = mvd[1][0] = mvd[1][1] = 0;
  merge_flag = 0;
  merge_idx = 0;
  mvp_lX_flag[0] = mvp_lX_flag[1] = 0;
  inter_pred_idc = 0;
  */

  /*
  enum IntraPredMode IntraPredModeC; // chroma intra-prediction mode for current CB
  */

  /*
  cu_transquant_bypass_flag = false;
  memset(transform_skip_flag,0, 3*sizeof(uint8_t));
  */


  //memset(coeffList,0,sizeof(int16_t)*3*32*32);
  //memset(coeffPos,0,sizeof(int16_t)*3*32*32);
  //memset(nCoeff,0,sizeof(int16_t)*3);



  IsCuQpDeltaCoded = false;
  CuQpDelta = 0;

  IsCuChromaQpOffsetCoded = false;
  CuQpOffsetCb = 0;
  CuQpOffsetCr = 0;

  /*
  currentQPY = 0;
  currentQG_x = 0;
  currentQG_y = 0;
  lastQPYinPreviousQG = 0;
  */

  /*
  qPYPrime = 0;
  qPCbPrime = 0;
  qPCrPrime = 0;
  */

  /*
  memset(&cabac_decoder, 0, sizeof(CABAC_decoder));
  memset(&ctx_model, 0, sizeof(ctx_model));
  */

  decctx = NULL;
  img = NULL;
  shdr = NULL;

  imgunit = NULL;
  sliceunit = NULL;


  //memset(this,0,sizeof(thread_context));

  // There is a interesting issue here. When aligning _coeffBuf to 16 bytes offset with
  // __attribute__((align(16))), the following statement is optimized away since the
  // compiler assumes that the pointer would be 16-byte aligned. However, this is not the
  // case when the structure has been dynamically allocated. In this case, the base can
  // also be at 8 byte offsets (at least with MingW,32 bit).
  int offset = ((uintptr_t)_coeffBuf) & 0xf;

  if (offset == 0) {
    coeffBuf = _coeffBuf;  // correctly aligned already
  }
  else {
    coeffBuf = (int16_t *) (((uint8_t *)_coeffBuf) + (16-offset));
  }

  memset(coeffBuf, 0, 32*32*sizeof(int16_t));
}