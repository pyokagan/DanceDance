activity = ['neutral', 'wavehands', 'busdriver', 'frontback', 'sidestep', 'jumping',
		'jumpingjack', 'turnclap', 'squatturnclap', 'window', 'window360']

def getActivityId(activityName):
	"""Given an activity name - associates it with an index (enumeration).

	Keyword arguments:
	activityName -- string identifying name of the dance move
	"""
	id = activity.index(activityName)
	if id == -1:
		raise ValueError('Unidentifiable activity!')
	else:
		return id

def getActivityName(activityId):
	"""Returns the dance move name of the provided id.

	Keyword arguments:
	activityId -- id of the dance move
	"""
	if -1 < activityId < len(activity):
		return activity[activityId]
	else:
		raise ValueError('Invalid Id generated')