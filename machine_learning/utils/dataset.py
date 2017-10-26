import numpy as np
import csv, glob
from activity import getActivityId
from segment import segment

raw_data = []

def split_x_y(segments):
	segment_x = []
	segment_y = []
	count = 0
	for segment in segments:
		if len(segment) > 0:
			segment_x.append([])
			for sample in segment:
				segment_x[count].append(sample[0:-1])
			segment_y.append(segment[0][-1])
			count += 1
	return segment_x, segment_y

def readCsvFile(filename):
	activity_name = (filename.split('/')[-1]).split('.')[0].split('-')[0]
	activity_id = getActivityId(activity_name)
	# print activity_name + ' %%'
	if activity_id < 0:
		return
	print(filename)
	with open(filename, 'rb') as csvfile:
		reader = csv.DictReader(csvfile)
		for row in reader:
			first_field = row['# acc1x'];
			if not first_field.startswith('#') and activity_id >= 0:
				raw_data.append([float(first_field), float(row[' acc1y']), float(row[' acc1z']), float(row[' gyro1x']), float(row[' gyro1y']), float(row[' gyro1z']),float(row[' acc2x']), float(row[' acc2y']), float(row[' acc2z']), float(row[' gyro2x']), float(row[' gyro2y']), float(row[' gyro2z']), activity_id])
	csvfile.close()
	print(len(raw_data))

def loadCsvFiles(folder):
	path = folder + '/*.csv'
	for filename in glob.glob(path):
		readCsvFile(filename)

def getData(foldername):
	loadCsvFiles(foldername)
	segmented_data = segment(raw_data)
	print(len(segmented_data))
	# print segmented_data[0]
	raw_x, raw_y = split_x_y(segmented_data)
	train_size = int(0.75 * len(raw_y))
	train_x, test_x = raw_x[0:train_size], raw_x[train_size: len(raw_y)]
	train_y, test_y = raw_y[0:train_size], raw_y[train_size: len(raw_y)]
	return np.array(train_x), np.array(train_y), np.array(test_x), np.array(test_y)
