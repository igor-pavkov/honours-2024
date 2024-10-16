void CLASS Canon_WBCTpresets(short WBCTversion)
{
  if (WBCTversion == 0)
    for (int i = 0; i < 15; i++) // tint, as shot R, as shot B, CСT
    {
      imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] = 1.0f;
      fseek(ifp, 2, SEEK_CUR);
      imgdata.color.WBCT_Coeffs[i][1] = 1024.0f / fMAX(get2(), 1.f);
      imgdata.color.WBCT_Coeffs[i][3] = 1024.0f / fMAX(get2(), 1.f);
      imgdata.color.WBCT_Coeffs[i][0] = get2();
    }
  else if (WBCTversion == 1)
    for (int i = 0; i < 15; i++) // as shot R, as shot B, tint, CСT
    {
      imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] = 1.0f;
      imgdata.color.WBCT_Coeffs[i][1] = 1024.0f / fMAX(get2(), 1.f);
      imgdata.color.WBCT_Coeffs[i][3] = 1024.0f / fMAX(get2(), 1.f);
      fseek(ifp, 2, SEEK_CUR);
      imgdata.color.WBCT_Coeffs[i][0] = get2();
    }
  else if ((WBCTversion == 2) && ((unique_id == 0x80000374) || // M3
                                  (unique_id == 0x80000384) || // M10
                                  (unique_id == 0x80000394) || // M5
                                  (unique_id == 0x80000407) || // M6
                                  (unique_id == 0x80000398) || // M100
                                  (unique_id == 0x03970000) || // G7 X Mark II
                                  (unique_id == 0x04100000) || // G9 X Mark II
                                  (unique_id == 0x04180000)))  // G1 X Mark III
    for (int i = 0; i < 15; i++)                               // tint, offset, as shot R, as shot B, CСT
    {
      fseek(ifp, 2, SEEK_CUR);
      fseek(ifp, 2, SEEK_CUR);
      imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] = 1.0f;
      imgdata.color.WBCT_Coeffs[i][1] = 1024.0f / fMAX(1.f, get2());
      imgdata.color.WBCT_Coeffs[i][3] = 1024.0f / fMAX(1.f, get2());
      imgdata.color.WBCT_Coeffs[i][0] = get2();
    }
  else if ((WBCTversion == 2) && ((unique_id == 0x03950000) || (unique_id == 0x03930000))) // G5 X, G9 X
    for (int i = 0; i < 15; i++) // tint, offset, as shot R, as shot B, CСT
    {
      fseek(ifp, 2, SEEK_CUR);
      fseek(ifp, 2, SEEK_CUR);
      imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] = 1.0f;
      imgdata.color.WBCT_Coeffs[i][1] = (float)get2() / 512.0f;
      imgdata.color.WBCT_Coeffs[i][3] = (float)get2() / 512.0f;
      imgdata.color.WBCT_Coeffs[i][0] = get2();
    }
  return;
}