void vbg_linux_mouse_event(struct vbg_dev *gdev)
{
	int rc;

	/* Report events to the kernel input device */
	gdev->mouse_status_req->mouse_features = 0;
	gdev->mouse_status_req->pointer_pos_x = 0;
	gdev->mouse_status_req->pointer_pos_y = 0;
	rc = vbg_req_perform(gdev, gdev->mouse_status_req);
	if (rc >= 0) {
		input_report_abs(gdev->input, ABS_X,
				 gdev->mouse_status_req->pointer_pos_x);
		input_report_abs(gdev->input, ABS_Y,
				 gdev->mouse_status_req->pointer_pos_y);
		input_sync(gdev->input);
	}
}