import sys
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
	if predictions[-1] == predictions[-2] == predictions[-3] == predictions[-4]:
		activityId = predictions[-1]
		print activityId
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
				print "result: ", result
				return result