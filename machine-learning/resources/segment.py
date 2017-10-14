import random

def get_window_size():
	"""Returns the size for each segment for input stream.

	Tells the Raspberry Pi to segment the input stream of data
	at intervals of 1 second.
	"""
	return 1.0

def has_pound(pound_holder):
	return type(pound_holder) is str and pound_holder.startswith('#')

def is_new_segment(new_label, old_label, pound_holder):
	return (new_label != old_label) or has_pound(pound_holder)

def segment(raw_data):
	segments = []
	current_segment = 0
	segments.append([raw_data[0]])
	for index in range(1, len(raw_data), 1):
		if is_new_segment(raw_data[index][-1], raw_data[index-1][-1], raw_data[index][0]):
			current_segment += 1
			segments.append([])
		if has_pound(raw_data[index][0]):
			continue
		segments[current_segment].append(raw_data[index])
	random.shuffle(segments)
	return segments