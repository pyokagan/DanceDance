# Required Packages
from sklearn import svm, neighbors
from sklearn.preprocessing import StandardScaler, Imputer
from sklearn.model_selection import KFold
from sklearn.metrics import accuracy_score, confusion_matrix
from resources import features, dataset
import numpy as np

# Here we assume that the input data is a list of segments,
# 	i.e; input = [segment0, segment1, segment2, ...]
# where each segment is composed of a series of readings - a reading at the freq of 42 Hz
# 	i.e; segment0 = [reading0, reading1, reading2, ...]
# Each reading consists of 12 values - 6 from each IMU (3 accelerometer, 3 gyroscope)
# 	i.e; reading0 = [IMU1, IMU2]
# 	   	 IMU1 = acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z

scaler = StandardScaler() # Standardization for ML model
clf_svm = svm.SVC(probability=True)
clf_knn = neighbors.KNeighborsClassifier(n_neighbors=6)
imputer = Imputer()

def cross_validate(outputFile, data_x, data_y, n_splits=4):
	outputFile.write('Cross Validation:\n')
	kf = KFold(n_splits=n_splits)
	svm_avg = 0
	knn_avg = 0
	for train_index, test_index in kf.split(data_x):
		X_train, X_test = data_x[train_index], data_x[test_index]
		y_train, y_test = data_y[train_index], data_y[test_index]
		clf_svm = svm.SVC()
		clf_svm.fit(X_train, y_train)
		svm_avg += clf_svm.score(X_test, y_test)
		clf_knn = neighbors.KNeighborsClassifier(n_neighbors=6)
		clf_knn.fit(X_train, y_train)
		knn_avg += clf_knn.score(X_test, y_test)
	outputFile.write("Avg SVM score:" + str(svm_avg/n_splits) + "\n")
	outputFile.write("Avg KNN score:" + str(knn_avg/n_splits) + "\n\n")

def evaluate_knn_classifier(test_input, test_output, outputFile):
	outputFile.write("Classifying using KNN:")
	test_input = extract_features(test_input)
	test_input = scaler.transform(test_input)
	# test_input = scaler.transform(imputer.fit_transform(test_input))
	test_prediction = clf_knn.predict(test_input)
	outputFile.write("Accuracy: " + str(accuracy_score(test_output, test_prediction)) + '\n')
	conf_matrix = confusion_matrix(test_output, test_prediction)
	outputFile.write("Confusion Matrix: " + str(conf_matrix))
	outputFile.write('\n\n')

def evaluate_svm_classifier(test_input, test_output, outputFile):
	outputFile.write("Classifying using SVM:")
	test_input = extract_features(test_input)
	test_input = scaler.transform(test_input)
	# test_input = scaler.transform(imputer.fit_transform(test_input))
	test_prediction = clf_svm.predict(test_input)
	outputFile.write("Accuracy: " + str(accuracy_score(test_output, test_prediction)) + '\n')
	conf_matrix = confusion_matrix(test_output, test_prediction)
	outputFile.write("Confusion Matrix: " + str(conf_matrix))
	outputFile.write('\n\n')

def extract_segment_features(segment, number_of_triaxes=2):
	values_transpose = segment.transpose()
	return features.getSampleFeatures(values_transpose, number_of_triaxes)

def extract_features(np_input):
	"""Extracts features from a given stream of sampled raw input.

	Keyword arguments:
	np_input -- numpy array object (ndarray) containing raw input
	"""
	features = []
	for segment in np_input:
		features.append(extract_segment_features(np.array(segment), 4))
	return np.array(features)

def train_svm_model(input_segment_list, output_dance_moves, outputFile):
	"""Trains an SVM model with the given inputs and outputs."""
	X = extract_features(np.array(input_segment_list))
	# X = imputer.fit_transform(X) # Consideration for missing data
	y = output_dance_moves
	scaler.fit(X)
	cross_validate(outputFile, scaler.transform(X), y, 4)
	clf_svm.fit(scaler.transform(X), y)
	clf_knn.fit(scaler.transform(X), y)

train_input, train_output, test_input, test_output = dataset.getData(foldername='../data2')
outputFile = open('outputs.txt', 'w')
train_svm_model(train_input, train_output, outputFile)
evaluate_svm_classifier(test_input, test_output, outputFile)
evaluate_knn_classifier(test_input, test_output, outputFile)
outputFile.close()

import pickle
from sklearn.externals import joblib
# saving models 
svm_file = "model_svm.sav"
joblib.dump(clf_svm, svm_file)

knn_file = "model_knn.sav"
joblib.dump(clf_knn, knn_file)

scaler_file = "scaler.save"
joblib.dump(scaler, scaler_file)