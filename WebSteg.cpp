#include "provided.h"
#include <string>
#include "http.h"
#include "HashTable.h"
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
	string pageContents;
	string message;
	if (HTTP().get(url, pageContents))
	{
		if (Steg::hide(pageContents, msg, message))//if it was able to hide it, set the string host to the edited message
		{
			host = message;
			HTTP().set(url, message);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
	string pageContents;
	if (HTTP().get(url, pageContents))
	{
		if (Steg::reveal(pageContents, msg))//if steg was able to reveal, return true
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
