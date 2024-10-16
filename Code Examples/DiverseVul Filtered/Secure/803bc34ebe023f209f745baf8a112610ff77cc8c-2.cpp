static toff_t TIFFGetBlobSize(thandle_t image)
{
  return((toff_t) GetBlobSize((Image *) image));
}