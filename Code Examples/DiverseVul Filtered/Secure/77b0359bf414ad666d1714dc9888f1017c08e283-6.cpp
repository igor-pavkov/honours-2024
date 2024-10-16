static int qemu_input_transform_invert_abs_value(int value)
{
  return (int64_t)INPUT_EVENT_ABS_MAX - value + INPUT_EVENT_ABS_MIN;
}