DLLEXPORT int tjGetErrorCode(tjhandle handle)
{
  tjinstance *this = (tjinstance *)handle;

  if (this && this->jerr.warning) return TJERR_WARNING;
  else return TJERR_FATAL;
}