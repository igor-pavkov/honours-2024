Guint FoFiTrueType::mapToVertGID(Guint orgGID)
{
  Guint mapped;

  if (gsubFeatureTable == 0) return orgGID;
  if ((mapped = doMapToVertGID(orgGID)) != 0) {
    return mapped;
  }
  return orgGID;
}