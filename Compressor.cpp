#include "provided.h"
#include <string>
#include <vector>
#include "HashTable.h"
using namespace std;
unsigned int computeHash(unsigned short ogre2)
{
	return ogre2;
}
unsigned int computeHash(string ogre1)
{
	int result = 0;
	for (int k = 0; k < ogre1.size(); k++){
		result += (ogre1[k] * (k + 1));
	}
	return result;
}
void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	string runSoFar;
	string tempX;
	string extendedRun;
	int nextFreeId = 256;
	numbers.clear();
	unsigned short value;
	unsigned short capacity = 0;
	int size = s.size();
	if ((size / 2 + 512) < 16384)//determines the capacity
		capacity = (size / 2 + 512);
	else
		capacity = 16384;
	HashTable<string, unsigned short> H(capacity * 2, capacity);
	string ascii;
	for (int k = 0; k < 256; k++)
	{
		ascii = k;//set the string to the value
		H.set(ascii, k, true);//set it in the hash table
	}
	for (int k = 0; k < s.size(); k++)
	{
		tempX = "";
		tempX += s[k];//have the char ahead in
		extendedRun = (runSoFar + s[k]);//add it to the string
		if (H.get(extendedRun, value))
		{//if it is found in the hash table
			runSoFar = extendedRun;//set runsofar to it
			continue;
		}
		else
		{
			unsigned short x;
			H.get(runSoFar, x);//get runsofar
			numbers.push_back(x);//push back runsofar
			H.touch(runSoFar);//set its priority and reset runsofar for new chars
			runSoFar = "";
			unsigned short cv;
			H.get(tempX, cv);
			numbers.push_back(cv);
			if (!H.isFull())
			{//if it was not full
				H.set(extendedRun, nextFreeId);//add this to the hash table
				nextFreeId++;
			}
			else
			{
				string discarded;
				unsigned short discardNumber;
				H.discard(discarded, discardNumber);//or else discard the oldest and set this new value
				H.set(extendedRun, discardNumber);
			}
			tempX = "";
		}
	}
	if (!runSoFar.empty())//if there is anything left in runsofar, add it in
	{
		unsigned short runLeft;
		H.get(runSoFar, runLeft);
		numbers.push_back(runLeft);
	}
	numbers.push_back(capacity);
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
	if (numbers.size() == 0)
	{
		return false;
	}
	s = "";
	string expandedRun;
	string runSoFar;
	string output;
	int nextFreeId = 256;
	HashTable<unsigned short, string> H(numbers[numbers.size() - 1] * 2, numbers[numbers.size() - 1]);
	string ascii;
	for (int k = 0; k < 256; k++)
	{
		ascii = k;
		H.set(k, ascii, true);
	}
	for (int k = 0; k < numbers.size() - 1; k++)//loop through the vecotr
	{
		if (numbers[k] <= 255)
		{
			string us;
			if (H.get(numbers[k], us))
				s += us;
			if (runSoFar.size() == 0)
			{
				runSoFar = us;//if runsofar is empty, add us in
				continue;
			}
			else
				expandedRun = (runSoFar + us);//if it is not empty, set expandedrun
			if (!H.isFull())
			{
				H.set(nextFreeId, expandedRun);//set this value
				nextFreeId++;
			}
			else
			{
				string discarded;
				unsigned short discardNumber;
				H.discard(discardNumber, discarded);
				H.set(discardNumber, expandedRun);
			}
			runSoFar = "";
		}
		else
		{
			string us;
			if (!H.get(numbers[k], us))
			{
				return false;//if false, return false
			}
			else
			{
				H.touch(numbers[k]);//touch the value
				s += us;
				runSoFar = us;//and set runsofar to this value
			}
		}

	}
	return true;
}
