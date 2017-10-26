import sys
from sklearn import svm, neighbors
from sklearn.externals import joblib
from sklearn.preprocessing import StandardScaler
from machine_learning.utils import activity, features
import numpy as np

import pickle
clf_svm = joblib.load("machine_learning/model_svm.sav")
clf_knn = joblib.load("machine_learning/model_knn.sav")
scaler = joblib.load("machine_learning/scaler.save")

def extract_segment_features(segment):
	values_transpose = segment.transpose()
	return features.getSampleFeatures(values_transpose)

def predict(raw_data):
	features = extract_segment_features(raw_data)
	features = scaler.transform(features.reshape(1,-1))
	output_svm = clf_svm.predict_proba(features)
	output_knn = clf_knn.predict_proba(features)
	if max(output_svm[0]) > 0.6:
		print("From SVM: " + str(output_svm))
		print("From KNN:" + str(output_knn))
		index = np.where(output_svm[0] == max(output_svm[0]))[0]
		return index + 1
	return -1

def get_final_prediction(predictions):
	print(predictions)
	if predictions[-1] != -1 and predictions[-1] == predictions[-2] == predictions[-3] == predictions[-4]:
		activityId = predictions[-1]
		print(activityId[0])
		return activity.getActivityName(activityId[0])
	return -1

def predict_segment(lines):
	segment = []
	predictions = []
	for line in lines:
		line = line.rstrip('\n')
		#print (line)
		sample = []
		if line.startswith('#'):
			if len(segment) == 0:
				continue
			segment_input = np.array(segment)
			predictions.append(predict(segment_input))
		else:
			sample = [float(x.strip()) for x in line.split(',')]
			segment.append(sample)

		if len(predictions) > 3:
			result = get_final_prediction(predictions)
			if result >= 0:
				print("result: " + result)
				return result
			else:
				return 'confused'
