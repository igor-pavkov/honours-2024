decoder_context::~decoder_context()
{
  while (!image_units.empty()) {
    delete image_units.back();
    image_units.pop_back();
  }
}