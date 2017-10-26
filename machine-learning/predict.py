import pickle, sys
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
	features = scaler.transform(features)
	raw_output = clf_svm.predict(features)
	raw_output1 = clf_knn.predict(features)
	print("From SVM: " + str(raw_output))
	print("From KNN:" + str(raw_output1))
	return raw_output

def get_final_prediction(predictions):
	maxKey = max(set(predictions), key=predictions.count)
	return activity.getActivityName(maxKey)

def predict_user_input():
	segment = []
	predictions = []
	while True:
		line = sys.stdin.readline().rstrip('\n')
		sample = []
		if line.startswith('#'):
			predictions.append(predict(segment))
			segment = []
			if len(predictions) > 10:
				yield get_final_prediction(predictions)
		else:
			sample = [float(x.strip()) for x in line.split(',')]
			segment.append(sample)

if __name__ == '__main__':
	predict_user_input()