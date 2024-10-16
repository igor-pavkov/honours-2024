LineBitmapRequester::LineBitmapRequester(class Frame *frame)
  : LineBuffer(frame), BitmapCtrl(frame), m_pEnviron(frame->EnvironOf()), m_pFrame(frame),
    m_pulReadyLines(NULL),
    m_ppDownsampler(NULL), m_ppUpsampler(NULL), m_ppTempIBM(NULL), 
    m_pppImage(NULL), m_bSubsampling(false)
{ 
  m_ucCount       = frame->DepthOf();
  m_ulPixelWidth  = frame->WidthOf();
  m_ulPixelHeight = frame->HeightOf();
}