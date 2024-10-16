inline kj::ArrayPtr<const word> SegmentBuilder::currentlyAllocated() {
  return kj::arrayPtr(ptr.begin(), pos - ptr.begin());
}