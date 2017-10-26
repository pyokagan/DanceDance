import sys
from collections import Counter
from sklearn import svm, neighbors
from sklearn.externals import joblib
from sklearn.preprocessing import StandardScaler
from resources import activity, features
import numpy as np

clf_svm = joblib.load("model_svm.sav")
clf_knn = joblib.load("model_knn.sav")
scaler = joblib.load("scaler.save")

def extract_segment_features(segment):
	values_transpose = segment.transpose()
	return features.getSampleFeatures(values_transpose)

def predict(raw_data):
	features = extract_segment_features(raw_data)
	features = scaler.transform(features.reshape(1,-1))
	raw_output = clf_svm.predict(features)
	raw_output1 = clf_knn.predict(features)
	print("From SVM: " + str(raw_output))
	print("From KNN:" + str(raw_output1))
	return raw_output[0]

def get_final_prediction(predictions):
	print predictions	
	count = Counter(predictions)
	activityId = count.most_common(1)[0][0]
	print activityId
	return activity.getActivityName(activityId)

def predict_segment(fileName):
	segment = []
	predictions = []
	lines = open(fileName).readlines()
	for line in lines:
		line = line.rstrip('\n')
		sample = []
		if line.startswith('#'):
			segment_input = np.array(segment)
			predictions.append(predict(segment_input))
			if len(predictions) > 8:
				result = get_final_prediction(predictions)
				print "result: ", result
				return result
		else:
			sample = [float(x.strip()) for x in line.split(',')]
			segment.append(sample)