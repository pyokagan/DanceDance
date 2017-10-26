import numpy as np
import math as math

def mean(input_list):
	"""Returns the average of the input values."""
	return np.mean(input_list)

def std(input_list):
	"""Returns the standard deviation in the input values."""
	return np.std(input_list)

def mad(input_list):
	"""Returns the median absolute deviation."""
	return np.median(input_list)

def max(input_list):
	"""Returns the largest value in the input array."""
	return np.max(input_list)

def min(input_list):
	"""Returns the smallest value in the input array."""
	return np.min(input_list)

def iqr(input_list):
	"""Returns the interquartile range."""
	return np.subtract(*np.percentile(input_list, [75, 25]))

def getSampleFeatures(raw_segment_transposed, number_of_triaxes = 2):
	"""Returns a list of features computed for raw_set.

	Keyword arguments:
	raw_segment_transposed -- Raw data set (segment) which is transposed
	number_of_sensors -- Number of IMUs being used (default = 1)
	"""
	features = []
	for index in range(len(raw_segment_transposed)):
		val_list = raw_segment_transposed[index]
		features.extend([mean(val_list), std(val_list), mad(val_list), max(val_list), min(val_list), iqr(val_list)])
	return np.array(features)