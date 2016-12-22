#include "provided.h"
#include <string>
#include "HashTable.h"
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut)
{
	hostOut = "";
	vector <unsigned short> v;
	vector <string> lines;
	string lineToAdd;
	string space;
	if (hostIn.empty())
		return false;
	for (int i = 0; i < hostIn.size(); i++)
	{
		if (i + 1 != hostIn.size() && hostIn[i] == '\r' && hostIn[i + 1] == '\n')
		{
			lines.push_back(lineToAdd);//if it is the case of \r\n, push back whatever was in lineToadd
			lineToAdd = "";//reset it 
			space = "";//reset the spaces
			i = i + 1;
		}
		else if (hostIn[i] == '\n')
		{
			lines.push_back(lineToAdd);//if it is a newline, reset lineToAdd
			lineToAdd = "";
			space = "";
		}
		else
		{
			if (hostIn[i] != 32 && hostIn[i] != 9)//if they are not tabs or spaces
			{
				lineToAdd += (space + hostIn[i]);//add in whatever spaces and the character
				space = "";
			}
			else
				space += hostIn[i];//otherwise add this to the space string to keep track of tabs and spaces
		}
	}
	if (lineToAdd.size() != 0)//if there is anything left, push it on
	{
		lines.push_back(lineToAdd);
	}
	Compressor::compress(msg, v);
	string encoded = BinaryConverter::encode(v);
	int L = encoded.length();
	int N = lines.size();
	int k = 0;
	int j = k;
	string temp;
	for (int i = 0; i < N; i++)
	{
		if (i < (L % N))//for this amount of items i nthe vector
		{
			for (k; k < j + 1 + (L / N); k++)
			{
				temp += encoded[k];//add a set amount of spaces and tabs to the end
			}
			j = k;
			lines[i] += temp;
			temp = "";//reset temp
		}
		else
		{
			for (k; k < j + L / N; k++)//if it has surpassed L%N you can only add L/N characters of spaces and tabs
			{
				temp += encoded[k];
			}
			j = k;
			lines[i] += temp;
			temp = "";
		}
	}
	for (int i = 0; i < lines.size(); i++)
	{
		hostOut += (lines[i] + '\n');//add these lines within the vector and add a space after each line
	}
	return true;
}

bool Steg::reveal(const string& hostIn, string& msg)
{
	vector <string> message;
	vector <unsigned short> decodeHolder;
	string temp;
	string holder;
	for (int i = 0; i < hostIn.size(); i++)
	{
		if (hostIn[i] == 32 || hostIn[i] == 9)//if its a space
		{
			temp += hostIn[i];//add it in to the temp
			continue;
		}
		if (hostIn[i] != 32 && hostIn[i] != 9 && hostIn[i] != '\n')
		{
			temp = "";
			continue;
		}
		if (hostIn[i] == '\n')//detection of a space indicates that this is end of sect of message
		{
			message.push_back(temp);
			temp = "";
		}
	}
	for (int i = 0; i < message.size(); i++)
	{
		holder += message[i];
	}
	if (BinaryConverter::decode(holder, decodeHolder))//decode 
	{
		if (Compressor::decompress(decodeHolder, holder))//decompress
		{
			msg = holder;//set the message to the result
			return true;
		}
	}
	return false;
}