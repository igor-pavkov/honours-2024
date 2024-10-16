composite_setup(struct usb_gadget *gadget, const struct usb_ctrlrequest *ctrl)
{
	struct usb_composite_dev	*cdev = get_gadget_data(gadget);
	struct usb_request		*req = cdev->req;
	int				value = -EOPNOTSUPP;
	int				status = 0;
	u16				w_index = le16_to_cpu(ctrl->wIndex);
	u8				intf = w_index & 0xFF;
	u16				w_value = le16_to_cpu(ctrl->wValue);
	u16				w_length = le16_to_cpu(ctrl->wLength);
	struct usb_function		*f = NULL;
	u8				endp;

	if (w_length > USB_COMP_EP0_BUFSIZ) {
		if (ctrl->bRequestType & USB_DIR_IN) {
			/* Cast away the const, we are going to overwrite on purpose. */
			__le16 *temp = (__le16 *)&ctrl->wLength;

			*temp = cpu_to_le16(USB_COMP_EP0_BUFSIZ);
			w_length = USB_COMP_EP0_BUFSIZ;
		} else {
			goto done;
		}
	}

	/* partial re-init of the response message; the function or the
	 * gadget might need to intercept e.g. a control-OUT completion
	 * when we delegate to it.
	 */
	req->zero = 0;
	req->context = cdev;
	req->complete = composite_setup_complete;
	req->length = 0;
	gadget->ep0->driver_data = cdev;

	/*
	 * Don't let non-standard requests match any of the cases below
	 * by accident.
	 */
	if ((ctrl->bRequestType & USB_TYPE_MASK) != USB_TYPE_STANDARD)
		goto unknown;

	switch (ctrl->bRequest) {

	/* we handle all standard USB descriptors */
	case USB_REQ_GET_DESCRIPTOR:
		if (ctrl->bRequestType != USB_DIR_IN)
			goto unknown;
		switch (w_value >> 8) {

		case USB_DT_DEVICE:
			cdev->desc.bNumConfigurations =
				count_configs(cdev, USB_DT_DEVICE);
			cdev->desc.bMaxPacketSize0 =
				cdev->gadget->ep0->maxpacket;
			if (gadget_is_superspeed(gadget)) {
				if (gadget->speed >= USB_SPEED_SUPER) {
					cdev->desc.bcdUSB = cpu_to_le16(0x0320);
					cdev->desc.bMaxPacketSize0 = 9;
				} else {
					cdev->desc.bcdUSB = cpu_to_le16(0x0210);
				}
			} else {
				if (gadget->lpm_capable)
					cdev->desc.bcdUSB = cpu_to_le16(0x0201);
				else
					cdev->desc.bcdUSB = cpu_to_le16(0x0200);
			}

			value = min(w_length, (u16) sizeof cdev->desc);
			memcpy(req->buf, &cdev->desc, value);
			break;
		case USB_DT_DEVICE_QUALIFIER:
			if (!gadget_is_dualspeed(gadget) ||
			    gadget->speed >= USB_SPEED_SUPER)
				break;
			device_qual(cdev);
			value = min_t(int, w_length,
				sizeof(struct usb_qualifier_descriptor));
			break;
		case USB_DT_OTHER_SPEED_CONFIG:
			if (!gadget_is_dualspeed(gadget) ||
			    gadget->speed >= USB_SPEED_SUPER)
				break;
			fallthrough;
		case USB_DT_CONFIG:
			value = config_desc(cdev, w_value);
			if (value >= 0)
				value = min(w_length, (u16) value);
			break;
		case USB_DT_STRING:
			value = get_string(cdev, req->buf,
					w_index, w_value & 0xff);
			if (value >= 0)
				value = min(w_length, (u16) value);
			break;
		case USB_DT_BOS:
			if (gadget_is_superspeed(gadget) ||
			    gadget->lpm_capable) {
				value = bos_desc(cdev);
				value = min(w_length, (u16) value);
			}
			break;
		case USB_DT_OTG:
			if (gadget_is_otg(gadget)) {
				struct usb_configuration *config;
				int otg_desc_len = 0;

				if (cdev->config)
					config = cdev->config;
				else
					config = list_first_entry(
							&cdev->configs,
						struct usb_configuration, list);
				if (!config)
					goto done;

				if (gadget->otg_caps &&
					(gadget->otg_caps->otg_rev >= 0x0200))
					otg_desc_len += sizeof(
						struct usb_otg20_descriptor);
				else
					otg_desc_len += sizeof(
						struct usb_otg_descriptor);

				value = min_t(int, w_length, otg_desc_len);
				memcpy(req->buf, config->descriptors[0], value);
			}
			break;
		}
		break;

	/* any number of configs can work */
	case USB_REQ_SET_CONFIGURATION:
		if (ctrl->bRequestType != 0)
			goto unknown;
		if (gadget_is_otg(gadget)) {
			if (gadget->a_hnp_support)
				DBG(cdev, "HNP available\n");
			else if (gadget->a_alt_hnp_support)
				DBG(cdev, "HNP on another port\n");
			else
				VDBG(cdev, "HNP inactive\n");
		}
		spin_lock(&cdev->lock);
		value = set_config(cdev, ctrl, w_value);
		spin_unlock(&cdev->lock);
		break;
	case USB_REQ_GET_CONFIGURATION:
		if (ctrl->bRequestType != USB_DIR_IN)
			goto unknown;
		if (cdev->config)
			*(u8 *)req->buf = cdev->config->bConfigurationValue;
		else
			*(u8 *)req->buf = 0;
		value = min(w_length, (u16) 1);
		break;

	/* function drivers must handle get/set altsetting */
	case USB_REQ_SET_INTERFACE:
		if (ctrl->bRequestType != USB_RECIP_INTERFACE)
			goto unknown;
		if (!cdev->config || intf >= MAX_CONFIG_INTERFACES)
			break;
		f = cdev->config->interface[intf];
		if (!f)
			break;

		/*
		 * If there's no get_alt() method, we know only altsetting zero
		 * works. There is no need to check if set_alt() is not NULL
		 * as we check this in usb_add_function().
		 */
		if (w_value && !f->get_alt)
			break;

		spin_lock(&cdev->lock);
		value = f->set_alt(f, w_index, w_value);
		if (value == USB_GADGET_DELAYED_STATUS) {
			DBG(cdev,
			 "%s: interface %d (%s) requested delayed status\n",
					__func__, intf, f->name);
			cdev->delayed_status++;
			DBG(cdev, "delayed_status count %d\n",
					cdev->delayed_status);
		}
		spin_unlock(&cdev->lock);
		break;
	case USB_REQ_GET_INTERFACE:
		if (ctrl->bRequestType != (USB_DIR_IN|USB_RECIP_INTERFACE))
			goto unknown;
		if (!cdev->config || intf >= MAX_CONFIG_INTERFACES)
			break;
		f = cdev->config->interface[intf];
		if (!f)
			break;
		/* lots of interfaces only need altsetting zero... */
		value = f->get_alt ? f->get_alt(f, w_index) : 0;
		if (value < 0)
			break;
		*((u8 *)req->buf) = value;
		value = min(w_length, (u16) 1);
		break;
	case USB_REQ_GET_STATUS:
		if (gadget_is_otg(gadget) && gadget->hnp_polling_support &&
						(w_index == OTG_STS_SELECTOR)) {
			if (ctrl->bRequestType != (USB_DIR_IN |
							USB_RECIP_DEVICE))
				goto unknown;
			*((u8 *)req->buf) = gadget->host_request_flag;
			value = 1;
			break;
		}

		/*
		 * USB 3.0 additions:
		 * Function driver should handle get_status request. If such cb
		 * wasn't supplied we respond with default value = 0
		 * Note: function driver should supply such cb only for the
		 * first interface of the function
		 */
		if (!gadget_is_superspeed(gadget))
			goto unknown;
		if (ctrl->bRequestType != (USB_DIR_IN | USB_RECIP_INTERFACE))
			goto unknown;
		value = 2;	/* This is the length of the get_status reply */
		put_unaligned_le16(0, req->buf);
		if (!cdev->config || intf >= MAX_CONFIG_INTERFACES)
			break;
		f = cdev->config->interface[intf];
		if (!f)
			break;
		status = f->get_status ? f->get_status(f) : 0;
		if (status < 0)
			break;
		put_unaligned_le16(status & 0x0000ffff, req->buf);
		break;
	/*
	 * Function drivers should handle SetFeature/ClearFeature
	 * (FUNCTION_SUSPEND) request. function_suspend cb should be supplied
	 * only for the first interface of the function
	 */
	case USB_REQ_CLEAR_FEATURE:
	case USB_REQ_SET_FEATURE:
		if (!gadget_is_superspeed(gadget))
			goto unknown;
		if (ctrl->bRequestType != (USB_DIR_OUT | USB_RECIP_INTERFACE))
			goto unknown;
		switch (w_value) {
		case USB_INTRF_FUNC_SUSPEND:
			if (!cdev->config || intf >= MAX_CONFIG_INTERFACES)
				break;
			f = cdev->config->interface[intf];
			if (!f)
				break;
			value = 0;
			if (f->func_suspend)
				value = f->func_suspend(f, w_index >> 8);
			if (value < 0) {
				ERROR(cdev,
				      "func_suspend() returned error %d\n",
				      value);
				value = 0;
			}
			break;
		}
		break;
	default:
unknown:
		/*
		 * OS descriptors handling
		 */
		if (cdev->use_os_string && cdev->os_desc_config &&
		    (ctrl->bRequestType & USB_TYPE_VENDOR) &&
		    ctrl->bRequest == cdev->b_vendor_code) {
			struct usb_configuration	*os_desc_cfg;
			u8				*buf;
			int				interface;
			int				count = 0;

			req = cdev->os_desc_req;
			req->context = cdev;
			req->complete = composite_setup_complete;
			buf = req->buf;
			os_desc_cfg = cdev->os_desc_config;
			w_length = min_t(u16, w_length, USB_COMP_EP0_OS_DESC_BUFSIZ);
			memset(buf, 0, w_length);
			buf[5] = 0x01;
			switch (ctrl->bRequestType & USB_RECIP_MASK) {
			case USB_RECIP_DEVICE:
				if (w_index != 0x4 || (w_value >> 8))
					break;
				buf[6] = w_index;
				/* Number of ext compat interfaces */
				count = count_ext_compat(os_desc_cfg);
				buf[8] = count;
				count *= 24; /* 24 B/ext compat desc */
				count += 16; /* header */
				put_unaligned_le32(count, buf);
				value = w_length;
				if (w_length > 0x10) {
					value = fill_ext_compat(os_desc_cfg, buf);
					value = min_t(u16, w_length, value);
				}
				break;
			case USB_RECIP_INTERFACE:
				if (w_index != 0x5 || (w_value >> 8))
					break;
				interface = w_value & 0xFF;
				if (interface >= MAX_CONFIG_INTERFACES ||
				    !os_desc_cfg->interface[interface])
					break;
				buf[6] = w_index;
				count = count_ext_prop(os_desc_cfg,
					interface);
				put_unaligned_le16(count, buf + 8);
				count = len_ext_prop(os_desc_cfg,
					interface);
				put_unaligned_le32(count, buf);
				value = w_length;
				if (w_length > 0x0A) {
					value = fill_ext_prop(os_desc_cfg,
							      interface, buf);
					if (value >= 0)
						value = min_t(u16, w_length, value);
				}
				break;
			}

			goto check_value;
		}

		VDBG(cdev,
			"non-core control req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);

		/* functions always handle their interfaces and endpoints...
		 * punt other recipients (other, WUSB, ...) to the current
		 * configuration code.
		 */
		if (cdev->config) {
			list_for_each_entry(f, &cdev->config->functions, list)
				if (f->req_match &&
				    f->req_match(f, ctrl, false))
					goto try_fun_setup;
		} else {
			struct usb_configuration *c;
			list_for_each_entry(c, &cdev->configs, list)
				list_for_each_entry(f, &c->functions, list)
					if (f->req_match &&
					    f->req_match(f, ctrl, true))
						goto try_fun_setup;
		}
		f = NULL;

		switch (ctrl->bRequestType & USB_RECIP_MASK) {
		case USB_RECIP_INTERFACE:
			if (!cdev->config || intf >= MAX_CONFIG_INTERFACES)
				break;
			f = cdev->config->interface[intf];
			break;

		case USB_RECIP_ENDPOINT:
			if (!cdev->config)
				break;
			endp = ((w_index & 0x80) >> 3) | (w_index & 0x0f);
			list_for_each_entry(f, &cdev->config->functions, list) {
				if (test_bit(endp, f->endpoints))
					break;
			}
			if (&f->list == &cdev->config->functions)
				f = NULL;
			break;
		}
try_fun_setup:
		if (f && f->setup)
			value = f->setup(f, ctrl);
		else {
			struct usb_configuration	*c;

			c = cdev->config;
			if (!c)
				goto done;

			/* try current config's setup */
			if (c->setup) {
				value = c->setup(c, ctrl);
				goto done;
			}

			/* try the only function in the current config */
			if (!list_is_singular(&c->functions))
				goto done;
			f = list_first_entry(&c->functions, struct usb_function,
					     list);
			if (f->setup)
				value = f->setup(f, ctrl);
		}

		goto done;
	}

check_value:
	/* respond with data transfer before status phase? */
	if (value >= 0 && value != USB_GADGET_DELAYED_STATUS) {
		req->length = value;
		req->context = cdev;
		req->zero = value < w_length;
		value = composite_ep0_queue(cdev, req, GFP_ATOMIC);
		if (value < 0) {
			DBG(cdev, "ep_queue --> %d\n", value);
			req->status = 0;
			composite_setup_complete(gadget->ep0, req);
		}
	} else if (value == USB_GADGET_DELAYED_STATUS && w_length != 0) {
		WARN(cdev,
			"%s: Delayed status not supported for w_length != 0",
			__func__);
	}

done:
	/* device either stalls (value < 0) or reports success */
	return value;
}