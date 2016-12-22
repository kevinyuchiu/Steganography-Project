#include "provided.h"
#include <string>
#include <vector>
#include "HashTable.h"
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;
string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);


string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
	string encodedMessage = "";
	for (int k = 0; k < numbers.size(); k++)
	{
		encodedMessage += convertNumberToBitString(numbers[k]);
	}
	for (int k = 0; k < encodedMessage.size(); k++)
	{
		if (encodedMessage[k] == '0')
			encodedMessage[k] = 32;
		else if (encodedMessage[k] == '1')
			encodedMessage[k] = 9;
	}
	return encodedMessage;
}

bool BinaryConverter::decode(const string& bitString,
	vector<unsigned short>& numbers)
{
	numbers.clear();
	string copy;
	for (int i = 0; i < bitString.size(); i++)
	{
		if (bitString[i] == 9)
			copy += '1';
		else if (bitString[i] == 32)
			copy += '0';
		else
			return false;
	}
	if (copy.size() % 16 != 0)
		return false;
	for (int k = 0; k < copy.size(); k += 16)
	{
		string temp = copy.substr(k, 16);
		unsigned short value;
		convertBitStringToNumber(temp, value);
		numbers.push_back(value);
	}
	return true;
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}

