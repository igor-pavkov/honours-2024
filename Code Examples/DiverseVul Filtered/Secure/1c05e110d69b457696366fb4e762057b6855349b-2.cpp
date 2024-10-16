void ModularFrameDecoder::MaybeDropFullImage() {
  if (full_image.transform.empty() && !have_something) {
    use_full_image = false;
    for (auto& ch : full_image.channel) {
      // keep metadata on channels around, but dealloc their planes
      ch.plane = Plane<pixel_type>();
    }
  }
}