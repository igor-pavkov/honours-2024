int LibRaw:: fc (int row, int col)
{
    static const char filter[16][16] =
        { { 2,1,1,3,2,3,2,0,3,2,3,0,1,2,1,0 },
          { 0,3,0,2,0,1,3,1,0,1,1,2,0,3,3,2 },
          { 2,3,3,2,3,1,1,3,3,1,2,1,2,0,0,3 },
          { 0,1,0,1,0,2,0,2,2,0,3,0,1,3,2,1 },
          { 3,1,1,2,0,1,0,2,1,3,1,3,0,1,3,0 },
          { 2,0,0,3,3,2,3,1,2,0,2,0,3,2,2,1 },
          { 2,3,3,1,2,1,2,1,2,1,1,2,3,0,0,1 },
          { 1,0,0,2,3,0,0,3,0,3,0,3,2,1,2,3 },
          { 2,3,3,1,1,2,1,0,3,2,3,0,2,3,1,3 },
          { 1,0,2,0,3,0,3,2,0,1,1,2,0,1,0,2 },
          { 0,1,1,3,3,2,2,1,1,3,3,0,2,1,3,2 },
          { 2,3,2,0,0,1,3,0,2,0,1,2,3,0,1,0 },
          { 1,3,1,2,3,2,3,2,0,2,0,1,1,0,3,0 },
          { 0,2,0,3,1,0,0,1,1,3,3,2,3,2,2,1 },
          { 2,1,3,2,3,1,2,1,0,3,0,2,0,2,0,2 },
          { 0,3,1,0,0,2,0,3,2,1,3,1,1,3,1,3 } };
    
    if (imgdata.idata.filters != 1) return FC(row,col);
    return filter[(row+imgdata.sizes.top_margin) & 15][(col+imgdata.sizes.left_margin) & 15];
}