import random

sampling_freq = 42

def get_window_size():
	"""Returns the size for each segment for input stream.

	Tells the Raspberry Pi to segment the input stream of data
	at intervals of 1 second.
	"""
	return 1.0

def has_pound(pound_holder):
	return type(pound_holder) is str and pound_holder.startswith('#')

def is_new_segment(new_label, old_label, current_id, first_in_segment_id):
	return (new_label != old_label) or (current_id - first_in_segment_id >= sampling_freq)

def segment(raw_data):
	"""Samples the raw input X and Y before it is used to train the SVM model.

	As the dataset is sampled at Nyquist frequency of 42Hz, divide into segments of 42 to make 1 second.
	"""
	segments = []
	current_segment = 0
	segments.append([raw_data[0]])  # Starting segment 0
	first_in_segment_id = 0
	for index in range(1, len(raw_data), 1):
		if is_new_segment(raw_data[index][-1], raw_data[index-1][-1], index, first_in_segment_id):
			# print "tueeee", index
			current_segment += 1
			first_in_segment_id = index
			segments.append([])
		segments[current_segment].append(raw_data[index])
	print len(segments)
	random.shuffle(segments)
	return segments