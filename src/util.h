#pragma once

#include <vector>
#include <map>
#include <netdb.h>
#include <fcntl.h>
#include "Logger.h"

using namespace std;

map<string, string> queryExtract();
vector<string> split(const string& source, const string& delim);
int getConnSocket(string host, string port);
