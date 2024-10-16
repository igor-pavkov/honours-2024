static ssize_t n_tty_read(struct tty_struct *tty, struct file *file,
			 unsigned char __user *buf, size_t nr)
{
	struct n_tty_data *ldata = tty->disc_data;
	unsigned char __user *b = buf;
	DECLARE_WAITQUEUE(wait, current);
	int c;
	int minimum, time;
	ssize_t retval = 0;
	long timeout;
	unsigned long flags;
	int packet;

	c = job_control(tty, file);
	if (c < 0)
		return c;

	/*
	 *	Internal serialization of reads.
	 */
	if (file->f_flags & O_NONBLOCK) {
		if (!mutex_trylock(&ldata->atomic_read_lock))
			return -EAGAIN;
	} else {
		if (mutex_lock_interruptible(&ldata->atomic_read_lock))
			return -ERESTARTSYS;
	}

	down_read(&tty->termios_rwsem);

	minimum = time = 0;
	timeout = MAX_SCHEDULE_TIMEOUT;
	if (!ldata->icanon) {
		minimum = MIN_CHAR(tty);
		if (minimum) {
			time = (HZ / 10) * TIME_CHAR(tty);
			if (time)
				ldata->minimum_to_wake = 1;
			else if (!waitqueue_active(&tty->read_wait) ||
				 (ldata->minimum_to_wake > minimum))
				ldata->minimum_to_wake = minimum;
		} else {
			timeout = (HZ / 10) * TIME_CHAR(tty);
			ldata->minimum_to_wake = minimum = 1;
		}
	}

	packet = tty->packet;

	add_wait_queue(&tty->read_wait, &wait);
	while (nr) {
		/* First test for status change. */
		if (packet && tty->link->ctrl_status) {
			unsigned char cs;
			if (b != buf)
				break;
			spin_lock_irqsave(&tty->link->ctrl_lock, flags);
			cs = tty->link->ctrl_status;
			tty->link->ctrl_status = 0;
			spin_unlock_irqrestore(&tty->link->ctrl_lock, flags);
			if (tty_put_user(tty, cs, b++)) {
				retval = -EFAULT;
				b--;
				break;
			}
			nr--;
			break;
		}
		/* This statement must be first before checking for input
		   so that any interrupt will set the state back to
		   TASK_RUNNING. */
		set_current_state(TASK_INTERRUPTIBLE);

		if (((minimum - (b - buf)) < ldata->minimum_to_wake) &&
		    ((minimum - (b - buf)) >= 1))
			ldata->minimum_to_wake = (minimum - (b - buf));

		if (!input_available_p(tty, 0)) {
			if (test_bit(TTY_OTHER_CLOSED, &tty->flags)) {
				up_read(&tty->termios_rwsem);
				tty_flush_to_ldisc(tty);
				down_read(&tty->termios_rwsem);
				if (!input_available_p(tty, 0)) {
					retval = -EIO;
					break;
				}
			} else {
				if (tty_hung_up_p(file))
					break;
				if (!timeout)
					break;
				if (file->f_flags & O_NONBLOCK) {
					retval = -EAGAIN;
					break;
				}
				if (signal_pending(current)) {
					retval = -ERESTARTSYS;
					break;
				}
				n_tty_set_room(tty);
				up_read(&tty->termios_rwsem);

				timeout = schedule_timeout(timeout);

				down_read(&tty->termios_rwsem);
				continue;
			}
		}
		__set_current_state(TASK_RUNNING);

		/* Deal with packet mode. */
		if (packet && b == buf) {
			if (tty_put_user(tty, TIOCPKT_DATA, b++)) {
				retval = -EFAULT;
				b--;
				break;
			}
			nr--;
		}

		if (ldata->icanon && !L_EXTPROC(tty)) {
			retval = canon_copy_from_read_buf(tty, &b, &nr);
			if (retval == -EAGAIN) {
				retval = 0;
				continue;
			} else if (retval)
				break;
		} else {
			int uncopied;
			/* The copy function takes the read lock and handles
			   locking internally for this case */
			uncopied = copy_from_read_buf(tty, &b, &nr);
			uncopied += copy_from_read_buf(tty, &b, &nr);
			if (uncopied) {
				retval = -EFAULT;
				break;
			}
		}

		n_tty_check_unthrottle(tty);

		if (b - buf >= minimum)
			break;
		if (time)
			timeout = time;
	}
	n_tty_set_room(tty);
	up_read(&tty->termios_rwsem);

	remove_wait_queue(&tty->read_wait, &wait);
	if (!waitqueue_active(&tty->read_wait))
		ldata->minimum_to_wake = minimum;

	mutex_unlock(&ldata->atomic_read_lock);

	__set_current_state(TASK_RUNNING);
	if (b - buf)
		retval = b - buf;

	return retval;
}