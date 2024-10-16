inline int32x2_t ComputeLerpx2(
    const qint32* top_left0, const qint32* top_right0,
    const qint32* bottom_left0, const qint32* bottom_right0,
    const qint32* top_left1, const qint32* top_right1,
    const qint32* bottom_left1, const qint32* bottom_right1,
    const int32* x_lerp, const int32x2_t y_lerpsx) {
  const int32x2_t x_lerpsx =
      X_LERP_SAME ? vld1_dup_s32(reinterpret_cast<const int32*>(x_lerp))
                  : vld1_s32(reinterpret_cast<const int32*>(x_lerp));

  const int32x2_t top_leftsx = ToInt32x2(top_left0, top_left1);
  const int32x2_t top_rightsx = ToInt32x2(top_right0, top_right1);
  const int32x2_t bottom_leftsx = ToInt32x2(bottom_left0, bottom_left1);
  const int32x2_t bottom_rightsx = ToInt32x2(bottom_right0, bottom_right1);

  const int32x2_t retval =
      ComputeLerp32x2<RESOLUTION>(top_leftsx, top_rightsx, bottom_leftsx,
                                  bottom_rightsx, x_lerpsx, y_lerpsx);
  return retval;
}