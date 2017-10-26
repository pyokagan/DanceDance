import sys
from sklearn import svm, neighbors
# from sklearn.externals import joblib
from sklearn.preprocessing import StandardScaler
from utils import activity, features
import numpy as np

import pickle
clf_svm = pickle.load(open("model_svm.sav", 'rb'))
clf_knn = pickle.load(open("model_knn.sav", 'rb'))
scaler = pickle.load(open("scaler.save", 'rb'))

def extract_segment_features(segment):
	values_transpose = segment.transpose()
	return features.getSampleFeatures(values_transpose)

def predict(raw_data):
	features = extract_segment_features(raw_data)
	features = scaler.transform(features.reshape(1,-1))
	output_svm = clf_svm.predict_proba(features)
	output_knn = clf_knn.predict_proba(features)
	if max(output_svm) > 0.6:
		print("From SVM: " + str(output_svm))
		print("From KNN:" + str(output_knn))
		return output_svm.index(max(output_svm)) + 1
	return -1

def get_final_prediction(predictions):
	print(predictions)
	if predictions[-1] == predictions[-2] == predictions[-3] == predictions[-4]:
		activityId = predictions[-1]
		print(activityId)
		return activity.getActivityName(activityId)
	return -1

def predict_segment(lines):
	segment = []
	predictions = []
	for line in lines:
		line = line.rstrip('\n')
		sample = []
		if line.startswith('#'):
			segment_input = np.array(segment)
			predictions.append(predict(segment_input))
		else:
			sample = [float(x.strip()) for x in line.split(',')]
			segment.append(sample)

		if len(predictions) > 4:
			result = get_final_prediction(predictions)
			if result >= 0:
				print("result: " + result)
				return result