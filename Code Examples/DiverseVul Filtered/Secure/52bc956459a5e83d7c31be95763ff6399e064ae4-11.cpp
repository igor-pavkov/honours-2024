inline void SegmentBuilder::checkWritable() {
  if (KJ_UNLIKELY(readOnly)) throwNotWritable();
}