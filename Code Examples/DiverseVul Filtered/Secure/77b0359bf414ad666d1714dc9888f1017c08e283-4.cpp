static void qemu_input_transform_abs_rotate(InputEvent *evt)
{
    InputMoveEvent *move = evt->u.abs.data;
    switch (graphic_rotate) {
    case 90:
        if (move->axis == INPUT_AXIS_X) {
            move->axis = INPUT_AXIS_Y;
        } else if (move->axis == INPUT_AXIS_Y) {
            move->axis = INPUT_AXIS_X;
            move->value = qemu_input_transform_invert_abs_value(move->value);
        }
        break;
    case 180:
        move->value = qemu_input_transform_invert_abs_value(move->value);
        break;
    case 270:
        if (move->axis == INPUT_AXIS_X) {
            move->axis = INPUT_AXIS_Y;
            move->value = qemu_input_transform_invert_abs_value(move->value);
        } else if (move->axis == INPUT_AXIS_Y) {
            move->axis = INPUT_AXIS_X;
        }
        break;
    }
}