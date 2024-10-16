void LineBitmapRequester::EncodeRegion(const RectAngle<LONG> &region)
{
  class ColorTrafo *ctrafo = ColorTrafoOf(true,false);
  int i;
  
  if (m_bSubsampling) { 
    RectAngle<LONG> r;
    ULONG minx   = region.ra_MinX >> 3;
    ULONG maxx   = region.ra_MaxX >> 3;
    ULONG miny   = region.ra_MinY >> 3;
    ULONG maxy   = region.ra_MaxY >> 3;
    ULONG x,y;

    // First part: Collect the data from
    // the user and push it into the color transformer buffer.
    // For that first build the downsampler.
    for(i = 0;i < m_ucCount;i++) {
      if (m_ppDownsampler[i]) {
        m_ppDownsampler[i]->SetBufferedRegion(region);
      }
    }
    
    for(y = miny,r.ra_MinY = region.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
      r.ra_MaxY = (r.ra_MinY & -8) + 7;
      if (r.ra_MaxY > region.ra_MaxY)
        r.ra_MaxY = region.ra_MaxY;
        
      for(x = minx,r.ra_MinX = region.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
        r.ra_MaxX = (r.ra_MinX & -8) + 7;
        if (r.ra_MaxX > region.ra_MaxX)
          r.ra_MaxX = region.ra_MaxX;
        
        for(i = 0;i < m_ucCount;i++) {
          // Collect the source data.
          ExtractBitmap(m_ppTempIBM[i],r,i);
        }
        
        //
        // Run the color transformer.
        ctrafo->RGB2YCbCr(r,m_ppTempIBM,m_ppCTemp);
        
        // Now push the transformed data into either the downsampler, 
        // or the forward DCT block row.
        for(i = 0;i < m_ucCount;i++) {
          if (m_ppDownsampler[i]) {
            // Just collect the data in the downsampler for the time
            // being. Will be taken care of as soon as it is complete.
            m_ppDownsampler[i]->DefineRegion(x,y,m_ppCTemp[i]);
          } else { 
            DefineRegion(x,Start8Lines(i),m_ppCTemp[i],i);
          }
        }
      }
      //
      // Advance the quantized rows for the non-subsampled components,
      // downsampled components will be advanced later.
      for(i = 0;i < m_ucCount;i++) {
        m_pulReadyLines[i]    += 8; // somehwere in the buffer.
        if (m_ppDownsampler[i] == NULL) {
          Next8Lines(i);
        } else {
          LONG bx,by;
          RectAngle<LONG> blocks;
          // Collect the downsampled blocks and push that into the DCT.
          m_ppDownsampler[i]->GetCollectedBlocks(blocks);
          for(by = blocks.ra_MinY;by <= blocks.ra_MaxY;by++) {
            struct Line *row = Start8Lines(i);
            for(bx = blocks.ra_MinX;bx <= blocks.ra_MaxX;bx++) {
              LONG src[64]; // temporary buffer, the DCT requires a 8x8 block
              m_ppDownsampler[i]->DownsampleRegion(bx,by,src);
              DefineRegion(bx,row,src,i);
            }
            m_ppDownsampler[i]->RemoveBlocks(by);
            Next8Lines(i);
          }
        }
      }
    }
  } else { // No downsampling required, residual coding possible.
    RectAngle<LONG> r;
    ULONG minx   = region.ra_MinX >> 3;
    ULONG maxx   = region.ra_MaxX >> 3;
    ULONG miny   = region.ra_MinY >> 3;
    ULONG maxy   = region.ra_MaxY >> 3;
    ULONG x,y;

    for(y = miny,r.ra_MinY = region.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
      r.ra_MaxY = (r.ra_MinY & -8) + 7;
      if (r.ra_MaxY > region.ra_MaxY)
        r.ra_MaxY = region.ra_MaxY;
        
      for(x = minx,r.ra_MinX = region.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
        r.ra_MaxX = (r.ra_MinX & -8) + 7;
        if (r.ra_MaxX > region.ra_MaxX)
          r.ra_MaxX = region.ra_MaxX;

        for(i = 0;i < m_ucCount;i++) {      
          ExtractBitmap(m_ppTempIBM[i],r,i);
        }
        
        ctrafo->RGB2YCbCr(r,m_ppTempIBM,m_ppCTemp);

        for(i = 0;i < m_ucCount;i++) {
          DefineRegion(x,Start8Lines(i),m_ppCTemp[i],i);
        }
      }
      for(i = 0;i < m_ucCount;i++) {
        Next8Lines(i);
        m_pulReadyLines[i]   += 8;
      }
    }
  }
}