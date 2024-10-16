image_unit::~image_unit()
{
  for (int i=0;i<slice_units.size();i++) {
    delete slice_units[i];
  }

  for (int i=0;i<tasks.size();i++) {
    delete tasks[i];
  }
}