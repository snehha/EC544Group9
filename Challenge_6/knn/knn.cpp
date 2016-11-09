#include "knn.h"

int euclidean(vector<int> a, vector<int> b, int length){
	int sum = 0;
	for(int i = 0; i < length; i++){
		sum += pow((a[i] - b[i]),2);
	}
	return sqrt(sum);
}

vector<int> totalEuclidean(vector<int> instance, vector<vector<int> > trainingSet, int k){
	vector<int> nearestKInd;
	vector<int> distances;
	vector<int> indeces;
	int lengthTrainingSet = trainingSet.size();
	int lengthInstance = instance.size();

	for(int i = 0; i < lengthTrainingSet; i++){
		int dist = euclidean(instance,trainingSet[i],lengthInstance);	//Return one int value for each
		distances[i] = dist; //Stores the distance between the instance and ith instance in the trainingSet
	}

	for(int i = 0; i < k; i++){
		int max = 0;
		int index;
		for(int j = 0; j < lengthTrainingSet; j++){
			if(distances[i] > max){
				max = distances[i];
				index = i;
			}
		}
		distances[index] = 0;
		indeces.push_back(index);
	}
	return indeces;
}

char* avgKNN(char** classLabels,vector<int> indeces){
	int size = indeces.size();

	map<char*,int> mostSeen;
	for(int i = 0; i < size; i++){
		if(mostSeen.find(classLabels[indeces[i]]) == mostSeen.end()){
			mostSeen[classLabels[indeces[i]]] = 1;
		}
		else{
			mostSeen[classLabels[indeces[i]]] += 1;
		}
	}

	char* maxLabel;
	int curMax = 0;
	for(map<char*,int>::iterator it = mostSeen.begin(); it != mostSeen.end(); it++){
		if(it->second > curMax){
			maxLabel = it->first;
			curMax = it->second;
		}
	}
	return maxLabel;
}

int main(){
	
	return 0;
}