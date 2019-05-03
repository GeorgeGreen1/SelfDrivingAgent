//-----------------------------------------------------------------------------
// Copyright (c) 2017 Ricardo David CM (http://ricardo-david.com),
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------


#include <iostream>
#include "AStar.h"
#include <windows.h>
#include "OrthogonalJPS.h"
#include <fstream>
#include <sstream>  
#include <string>
#include <cmath>
#include <vector>
#include <windows.h>
#include <time.h>
#include "Vector2D.h"
#include "conio.h"
#define M_PI 3.14159265358979323846
#pragma warning(disable:4996)
//#define WINVER 0x0500

using namespace std;

bool arf = false;

int arrX = 30;
int arrY = 30;

vector<string> str_split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

void waitToFinish() {
	char user_exit;
	std::cout << "Press any key to exit:";
	while (!kbhit())
	{
	}
}

/*bool validBlock(int x, int y) {
/*bool validBlock(int x, int y) {
	if (!(x < 0 || x>7 || y < 0 || y>5)) {
		return blocks[x][y];
	}
	return false;
}*/
Vector2D currentBlock(float x, float y) {
	float ret_x = (floor((x - 2218.007568) / 40)) + 15;
	float ret_y = (floor((y - 1344.315063) / 40)) + 15;
	return Vector2D(ret_x,ret_y);
}

Vector2D mapCoords(float x, float y) {
	float ret_x = (x - 2218.007568) + 15 * 40;
	float ret_y = (y - 1344.315063) + 15 * 40;
	return Vector2D(ret_x, ret_y);
}

/*const unsigned char * unfoldMulti(unsigned char r[30][30]) {
	const unsigned char * newArr = new unsigned char[30 * 30];
	for (int i = 0; i < 30; i++) {
		copy(r[i], r[i] + 30, newArr);
	}
	return newArr;
}*/

Vector2D getTrueBlocks(int i) {
	int xBlock = i % 30;
	int yBlock = i / 30;
	return Vector2D(xBlock,yBlock);
}

Vector2D * getAllTrueBlocks(int n, int buffer[]) {
	Vector2D *instr = new Vector2D[n];
	Vector2D curr_vect;
	for (int i = 0; i < n; i++) {
		curr_vect = getTrueBlocks(buffer[i]);
		instr[i] = Vector2D(curr_vect.get_x(),curr_vect.get_y());
	}
	return instr;
}

int getBufArrPos(int x, int y) {
	return (y * 30) + x;
}


Vector2D getMidCoords(float x, float y) {
	return Vector2D((x * 40) + 20, (y * 40) + 20);
}



bool speedControl(Vector2D curr, Vector2D prev, bool going, float target) {
	//std::cout << curr.get_x() << ", " << curr.get_y() << " | " << prev.get_x() << ", " << prev.get_y() << " \n";
	float speed = sqrt(pow((curr.get_x() - prev.get_x()), 2) + pow((curr.get_y() - prev.get_y()), 2)) / 1000 * 36000;
	bool goes = going;
	if ((speed < target) && (!going)) {
		system("C:\\Users\\Jake\\source\\repos\\pathfinding-c\\x64\\Debug\\greatqb.bat 1");
		goes = true;
	}
	if ((speed > (target + 2)) && (going)) {
		system("C:\\Users\\Jake\\source\\repos\\pathfinding-c\\x64\\Debug\\greatqb.bat 0");
		goes = false;
	}
	return goes;
}

int steering(float angle,int steering) {
	int steers = steering;
	string open = "open";
	string right = "C:\\Program Files\\AutoHotkey\\scripts\\rightsteerOn.exe";
	string left = "C:\\Program Files\\AutoHotkey\\scripts\\leftsteerOn.exe";
	string off = "C:\\Program Files\\AutoHotkey\\scripts\\steerOff.exe";
	wstring open_temp = wstring(open.begin(), open.end());
	wstring right_temp = wstring(right.begin(), right.end());
	wstring left_temp = wstring(left.begin(), left.end());
	wstring off_temp = wstring(off.begin(), off.end());
	LPCWSTR openw = open_temp.c_str();
	LPCWSTR rightw = right_temp.c_str();
	LPCWSTR leftw = left_temp.c_str();
	LPCWSTR offw = off_temp.c_str();
	if ((angle > 0.4)&&(angle < ((2 * M_PI) - 0.4))) {
		if (angle < M_PI) {
			if (steering == -1) {
				ShellExecute(0, openw, offw, NULL, NULL, SW_SHOWNORMAL);
			}
			steers = 1;
			ShellExecute(0, openw, rightw, NULL, NULL, SW_SHOWNORMAL);
		}
		else {
			//std::cout << "Left Down \n";
			if (steering == 1) {
				ShellExecute(0, openw, offw, NULL, NULL, SW_SHOWNORMAL);
			}
			steers = -1;
			ShellExecute(0, openw, leftw, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	else if ((steering)&& (((angle <= 0.4) || (angle >= ((2 * M_PI) - 0.4))))) {
		steers = 0;
		ShellExecute(0, openw, offw, NULL, NULL, SW_SHOWNORMAL);
	}
	return steers;
}

void PrintSolution(int n, int* pOutBuffer)
{
	if (n == -1)
	{
		std::cout << "no path found!" << std::endl;
		return;
	}

	std::cout << "n Elements : " << n << std::endl;
	for (int i = 0; i < n; ++i)
		std::cout << "move : " << getTrueBlocks(pOutBuffer[i]).get_x() << ", " << getTrueBlocks(pOutBuffer[i]).get_y() << std::endl;
}


int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	PathFinding::OrthogonalJPS jps;
	return jps.GetPath({ nStartX, nStartY }, { nTargetX, nTargetY }, { pMap, nMapWidth, nMapHeight }, pOutBuffer, nOutBufferSize);

	//PathFinding::AStar astar;
	//return astar.GetPath({ nStartX, nStartY }, { nTargetX, nTargetY }, { pMap, nMapWidth, nMapHeight }, pOutBuffer, nOutBufferSize);
}


void maze() {
	string line;
	FILE * pFile;
	float in_arr[4];
	vector<string> sep;
	unsigned char pMap[] =
	{ 1,1,0,1,1,1,0,1,0,0,1,0,1,0,1,1,1,0,1,1,1,1,0,1,0,1,1,1,1,1,
 0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,0,1,0,1,0,1,0,1,0,0,0,1,
 1,1,1,1,1,0,0,1,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,1,0,1,1,0,0,0,
 1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,1,0,1,
 0,0,1,0,0,0,1,0,1,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,1,0,0,1,0,1,
 1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,0,0,0,1,0,0,1,1,1,1,
 0,0,1,1,0,0,1,1,1,1,0,0,0,0,1,0,1,0,1,1,1,1,0,0,1,0,1,1,0,1,
 1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,
 1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,0,0,0,1,0,0,1,1,1,0,
 0,0,0,1,0,0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,0,1,
 1,1,0,1,1,0,1,1,1,1,0,1,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,1,1,
 1,0,1,0,0,1,0,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,0,0,1,1,0,1,
 1,1,1,0,1,0,1,1,0,1,0,0,0,1,0,0,1,1,1,0,1,1,0,1,1,1,1,0,0,1,
 0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,
 1,1,0,1,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,0,1,1,0,1,0,
 0,0,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,1,1,0,1,
 1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
 0,1,0,0,1,1,1,1,0,0,1,1,0,0,0,1,0,1,1,1,0,1,1,0,1,1,1,0,0,1,
 1,0,0,0,1,0,1,1,0,1,0,0,0,1,1,1,1,0,1,1,1,0,1,0,0,1,0,1,1,0,
 0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,0,1,0,0,1,0,0,1,1,0,1,0,1,
 1,0,1,1,1,1,1,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,0,0,0,1,1,0,0,
 1,1,0,1,0,0,1,0,0,0,1,1,1,1,0,1,1,1,1,1,1,0,0,0,1,1,0,1,1,0,
 1,1,0,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,0,1,0,
 1,1,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,0,0,1,0,1,0,1,1,1,1,1,1,0,
 1,0,1,1,1,1,1,0,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,
 1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,0,1,0,0,1,1,1,1,
 1,1,1,1,1,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,0,0,1,1,0,0,
 1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,0,0,0,0,1,1,0,1,1,1,0,0,1,1,
 1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,0,0,0,1,0,0,0,1,1,0,
 1,1,0,1,1,1,1,1,0,1,1,0,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1 };
	int pOutBuffer[99];
	int n = FindPath(0, 0, 29, 29, pMap, 30, 30, pOutBuffer, 99);
	clock_t start_pathfind = clock();
	clock_t end_pathfind = (clock() - start_pathfind);
	std::cout << "Total Course Pathfinding Time: " << (end_pathfind) << "ms \n";
	Vector2D *coords = new Vector2D[n];
	Vector2D prev = Vector2D(0, 0);
	Vector2D last_block = Vector2D(0, 0);
	coords = getAllTrueBlocks(n, pOutBuffer);
	int x_block = 0, y_block = 0, courseWidth = 1200, courseHeight = 1200,
		prev_x_block = 0, readCounter = 0, path_point = 0, bytes_read, mistakes = 0;
	float prev_x = 0, prev_y = 0, speed;

	clock_t last_read = clock();
	bool going = false, reached_dest = false;
	int steer = 0;
	for (int n = 0; n < 3; n++) {
		std::cout << 3 - n << "\n";
		Sleep(1000);
	}
	std::cout << "GO! \n";
	clock_t start_read = clock();
	clock_t travel_read;
	while (path_point < n) {
		Sleep(50);
		pFile = fopen("C:\\dev\\output\\output.dat", "r");
		bytes_read = fread(in_arr, sizeof(float), sizeof(in_arr), pFile);
		Vector2D block = currentBlock(in_arr[0], in_arr[1]);
		Vector2D dir = Vector2D(in_arr[2], in_arr[3]);
		fclose(pFile);
		Vector2D realCoords = getMidCoords(coords[path_point].get_x(), coords[path_point].get_y());
		Vector2D targetDist = realCoords - mapCoords(in_arr[0], in_arr[1]);
		targetDist.normalize();
		dir.normalize();
		//std::cout << targetDist.get_x() << ", " << targetDist.get_y() << "\n";
		float diffAngle = dir.angleTo(targetDist);
		//	std::cout << diffAngle << "\n";
			/*if (diffAngle > 0.1) {
				if (dir.get_x() < targetDist.get_x()) {
					std::cout << "LEFT \n";
				}
				else {
					std::cout << "RIGHT \n";
				}
			}*/
		if ((block.get_x() == coords[path_point].get_x()) && (block.get_y() == coords[path_point].get_y())) {
			path_point++;
		}
		if ((block.get_x() != last_block.get_x()) || (block.get_y() != last_block.get_y())) {
			last_block = block;
			if (pMap[getBufArrPos(block.get_x(), block.get_y())] == 0) {
				std::cout << "Ospie \n";
				mistakes++;
			}
		}
		if (readCounter == 2) {
			if (steer != 0) {
				going = speedControl(Vector2D(in_arr[0], in_arr[1]), prev, going, 10);
			}
			else {
				going = speedControl(Vector2D(in_arr[0], in_arr[1]), prev, going, 40);
			}
			prev = Vector2D(in_arr[0], in_arr[1]);
			readCounter = 0;
		}
		steer = steering(diffAngle, steer);
		readCounter++;
		last_read = clock();
	}
	travel_read = (clock() - start_read);
	std::cout << "Total Course Traversal Time: " << (travel_read) << "ms \n";
	std::cout << "Mistakes:  " << mistakes << "\n";
	Sleep(60000);
}

void minPath() {
	Sleep(2000);
	string line;
	FILE * pFile;
	float in_arr[4];;
	vector<string> sep;
	unsigned char pMap[] =
  { 0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,0,0,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	int pOutBuffer[99];
	std::cout << "D";
	clock_t start_pathfind = clock();
	int n = FindPath(1, 0, 18, 12, pMap, 30, 30, pOutBuffer, 99);
	clock_t end_pathfind = (clock() - start_pathfind);
	std::cout << "Total Course Pathfinding Time: " << (end_pathfind) << "ms \n";
	std::cout << "E";
	std::cout << n << "\n";
	Vector2D *coords = new Vector2D[n];
	Vector2D prev = Vector2D(0, 0);
	Vector2D last_block = Vector2D(0, 0);
	coords = getAllTrueBlocks(n, pOutBuffer);
	int x_block = 0, y_block = 0, courseWidth = 1200, courseHeight = 1200,
		prev_x_block = 0, readCounter = 0, path_point = 0, steer = 0, block_count = 1, bytes_read, mistakes = 0,
		path_length = 0;
	float prev_x = 0, prev_y = 0, speed;
	bool going = false;
	clock_t start_read = clock();
	clock_t travel_read;
	while (path_point < n) {
		Sleep(50);
		pFile = fopen("C:\\dev\\output\\output.dat", "r");
		bytes_read = fread(in_arr, sizeof(float), sizeof(in_arr), pFile);
		Vector2D block = currentBlock(in_arr[0], in_arr[1]);
		Vector2D dir = Vector2D(in_arr[2], in_arr[3]);
		fclose(pFile);
		Vector2D realCoords = getMidCoords(coords[path_point].get_x(), coords[path_point].get_y());
		Vector2D targetDist = realCoords - mapCoords(in_arr[0], in_arr[1]);
		targetDist.normalize();
		dir.normalize();
		//std::cout << targetDist.get_x() << ", " << targetDist.get_y() << "\n";
		float diffAngle = dir.angleTo(targetDist);
		//	std::cout << diffAngle << "\n";
			/*if (diffAngle > 0.1) {
				if (dir.get_x() < targetDist.get_x()) {
					std::cout << "LEFT \n";
				}
				else {
					std::cout << "RIGHT \n";
				}
			}*/
		if ((block.get_x() == coords[path_point].get_x()) && (block.get_y() == coords[path_point].get_y())) {
			path_point++;
			block_count++;
		}
		if ((block.get_x() != last_block.get_x()) || (block.get_y() != last_block.get_y())) {
			last_block = block;
			path_length++;
			if (pMap[getBufArrPos(block.get_x(), block.get_y())] == 0) {
				std::cout << "Ospie \n";
				mistakes++;
			}
		}
		if (readCounter == 2) {
			if (steer != 0) {
				going = speedControl(Vector2D(in_arr[0], in_arr[1]), prev, going, 10);
			}
			else {
				going = speedControl(Vector2D(in_arr[0], in_arr[1]), prev, going, 40);
			}
			prev = Vector2D(in_arr[0], in_arr[1]);
			readCounter = 0;
		}
		steer = steering(diffAngle, steer);
		readCounter++;
	}
	travel_read = (clock() - start_read);
	std::cout << "Total Course Traversal Time: " << (travel_read) << "ms \n";
	std::cout << "Mistakes:  " << mistakes << "\n";
	std::cout << "Path Length:  " << path_length << "\n";
	Sleep(60000);
}


void memory() {
	string line;
	FILE * pFile;
	float in_arr[4];
	vector<string> sep;
	Vector2D targets[10] = {Vector2D(0,0)};
	Vector2D targetVec[5] = { Vector2D(6,5), Vector2D(8,5), Vector2D(10,5), Vector2D(12,5), Vector2D(14,5) };
	srand(194011);
	std::cout << "Target Points: \n";
	for (int i = 0; i < 10; i++) {
		int point = rand() % 5;
		targets[i] = targetVec[point];
		std::cout << point+1 << "\n";
	}
	Sleep(2000);
	int targPoint = 0;
	unsigned char pMap[] =
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	int pOutBuffer[20];
	float prev_x = 0, prev_y = 0, speed;
	bool going = false;
	std::cout << "Get Ready... \n";
	Sleep(2000);
	for (int n = 0; n < 3; n++) {
		std::cout << 3 - n << "\n";
		Sleep(1000);
	}
	std::cout << "GO! \n";
	clock_t start_pathfind = clock();
	clock_t start_read = clock();
	clock_t travel_read;
	int n = FindPath(6, 2, targets[targPoint].get_x(), targets[targPoint].get_y(), pMap, 30, 30, pOutBuffer, 99);
	clock_t end_pathfind = (clock() - start_pathfind);
	std::cout << "Total Course Pathfinding Time: " << (end_pathfind) << "ms \n";
	Sleep(10000);
	Vector2D *coords = new Vector2D[n];
	Vector2D prev = Vector2D(0, 0);
	Vector2D last_block = Vector2D(0, 0);
	coords = getAllTrueBlocks(n, pOutBuffer);
	int x_block = 0, y_block = 0, courseWidth = 1200, courseHeight = 1200,
		prev_x_block = 0, readCounter = 0, path_point = 0, steer = 0, block_count = 1, bytes_read, mistakes = 0;
	while (targPoint < 10) {
		Sleep(50);
		pFile = fopen("C:\\dev\\output\\output.dat", "r");
		bytes_read = fread(in_arr, sizeof(float), sizeof(in_arr), pFile);
		Vector2D block = currentBlock(in_arr[0], in_arr[1]);
		Vector2D dir = Vector2D(in_arr[2], in_arr[3]);
		fclose(pFile);
		Vector2D realCoords = getMidCoords(coords[path_point].get_x(), coords[path_point].get_y());
		Vector2D targetDist = realCoords - mapCoords(in_arr[0], in_arr[1]);
		targetDist.normalize();
		dir.normalize();
		float diffAngle = dir.angleTo(targetDist);
		if ((block.get_x() == coords[path_point].get_x()) && (block.get_y() == coords[path_point].get_y())) {
			path_point++;
		}
		if (readCounter == 2) {
			if (steer != 0) {
				going = speedControl(Vector2D(in_arr[0], in_arr[1]), prev, going, 10);
			}
			else {
				going = speedControl(Vector2D(in_arr[0], in_arr[1]), prev, going, 40);
			}
			prev = Vector2D(in_arr[0], in_arr[1]);
			readCounter = 0;
		}
		steer = steering(diffAngle, steer);
		readCounter++;
		// Check if the vehicle is either on an obstacle or the wrong target block
		if ((block.get_x() != last_block.get_x()) || (block.get_y() != last_block.get_y())) {
			last_block = block;
			if ((pMap[getBufArrPos(block.get_x(), block.get_y())] == 0) ||
				((block.get_y() == 5) && (block.get_x() != targets[targPoint].get_x()))) {
				std::cout << "Ospie \n";
				mistakes++;
			}
		}
		if ((block.get_x() == targets[targPoint].get_x()) && (block.get_y() == targets[targPoint].get_y())) {
			targPoint++;
			if (targPoint < 10) {
				path_point = 0;
				n = FindPath(block.get_x(), block.get_y(), targets[targPoint].get_x(), targets[targPoint].get_y(), pMap, 30, 30, pOutBuffer, 20);
				coords = getAllTrueBlocks(n, pOutBuffer);
			}
		}
	}
	travel_read = (clock() - start_read);
	std::cout << "Total Course Traversal Time: " << (travel_read) << "ms \n";
	std::cout << "Mistakes:  " << mistakes << "\n";
	Sleep(60000);
}

void memoryManual() {
	string line;
	FILE * pFile;
	float in_arr[4];
	vector<string> sep;
	Vector2D targets[10] = { Vector2D(0,0) };
	Vector2D targetVec[5] = { Vector2D(6,5), Vector2D(8,5), Vector2D(10,5), Vector2D(12,5), Vector2D(14,5) };
	Vector2D last_block = Vector2D(0, 0);
	Sleep(2000);
	unsigned char pMap[] =
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	std::cout << "Target Points: \n";
	srand(194011);
	int lastPt = -1;
	for (int i = 0; i < 10; i++) {
		int point = rand() % 5;
		if (i > 1) {
			while (point == lastPt) {
				point = rand() % 5;
			}
		}
		targets[i] = targetVec[point];
		lastPt = point;
		std::cout << point + 1 << "\n";
	}
	Sleep(4000);
	for (int j = 0; j < 100; j++) {
		std::cout << "~ \n";
	}
	for (int n = 0; n < 3; n++) {
		std::cout << 3 - n << "\n";
		Sleep(1000);
	}
	std::cout << "GO! \n";
	clock_t start_read = clock();
	clock_t travel_read;
	int targPoint = 0;
	int x_block = 0, y_block = 0, courseWidth = 1200, courseHeight = 1200,
		prev_x_block = 0, readCounter = 0, path_point = 0, steer = 0, block_count = 1, bytes_read, mistakes = 0;
	float prev_x = 0, prev_y = 0, speed;
	bool going = false;
	while (targPoint < 10) {
		Sleep(50);
		pFile = fopen("C:\\dev\\output\\output.dat", "r");
		bytes_read = fread(in_arr, sizeof(float), sizeof(in_arr), pFile);
		Vector2D block = currentBlock(in_arr[0], in_arr[1]);
		fclose(pFile);
		// Check if the vehicle is either on an obstacle or the wrong target block
		if ((block.get_x() != last_block.get_x()) || (block.get_y() != last_block.get_y())) {
			last_block = block;
			if ((pMap[getBufArrPos(block.get_x(), block.get_y())] == 0) ||
				((block.get_y() == 5) && (block.get_x() != targets[targPoint].get_x()))) {
				std::cout << "Ospie \n";
				mistakes++;
			}
		}
		if ((block.get_x() == targets[targPoint].get_x()) && (block.get_y() == targets[targPoint].get_y())) {
			std::cout << "Target " << targPoint << " found! \n";
			targPoint++;
		}
	}
	travel_read = (clock() - start_read);
	std::cout << "Total Course Traversal Time: " << (travel_read) << "ms \n";
	std::cout << "Mistakes:  " << mistakes << "\n";
	Sleep(60000);
}

void minPathManual() {
	Sleep(2000);
	string line;
	FILE * pFile;
	float in_arr[4];
	vector<string> sep;
	unsigned char pMap[] =
	{ 0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,0,0,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	int x_block = 0, y_block = 0, courseWidth = 1200, courseHeight = 1200,
		prev_x_block = 0, readCounter = 0, path_point = 0, path_length = 0, block_count = 1, bytes_read, mistakes = 0;
	float prev_x = 0, prev_y = 0, speed;
	bool complete = false;
	clock_t last_read = clock();
	Vector2D last_block = Vector2D(0, 0);
	std::cout << "Get Ready... \n";
	Sleep(2000);
	for (int n = 0; n < 3; n++) {
		std::cout << 3 - n << "\n";
		Sleep(1000);
	}
	std::cout << "GO! \n";
	while (!complete) {
		Sleep(50);
		pFile = fopen("C:\\dev\\output\\output.dat", "r");
		bytes_read = fread(in_arr, sizeof(float), sizeof(in_arr), pFile);
		Vector2D block = currentBlock(in_arr[0], in_arr[1]);
		if ((block.get_x() != last_block.get_x()) || (block.get_y() != last_block.get_y())) {
			last_block = block;
			path_length++;
			cout << "New \n";
			if (pMap[getBufArrPos(block.get_x(), block.get_y())] == 0) {
				std::cout << "Ospie \n";
				mistakes++;
			}
		}
		fclose(pFile);
		if ((block.get_x() == 18) && (block.get_y() == 12)) {
			complete = true;
		}
	}
	std::cout << "Blocks Covered: " << path_length-1 << "\n";
	std::cout << "Mistakes:  " << mistakes << "\n";
	Sleep(60000);
}

void mazeManual() {
	string line;
	FILE * pFile;
	float in_arr[4];
	vector<string> sep;
	unsigned char pMap[] =
	{ 1,1,0,1,1,1,0,1,0,0,1,0,1,0,1,1,1,0,1,1,1,1,0,1,0,1,1,1,1,1,
 0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,0,1,0,1,0,1,0,1,0,0,0,1,
 1,1,1,1,1,0,0,1,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,1,0,1,1,0,0,0,
 1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,1,0,1,
 0,0,1,0,0,0,1,0,1,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,1,0,0,1,0,1,
 1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,0,0,0,1,0,0,1,1,1,1,
 0,0,1,1,0,0,1,1,1,1,0,0,0,0,1,0,1,0,1,1,1,1,0,0,1,0,1,1,0,1,
 1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,
 1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,0,0,0,1,0,0,1,1,1,0,
 0,0,0,1,0,0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,0,1,
 1,1,0,1,1,0,1,1,1,1,0,1,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,1,1,
 1,0,1,0,0,1,0,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,0,0,1,1,0,1,
 1,1,1,0,1,0,1,1,0,1,0,0,0,1,0,0,1,1,1,0,1,1,0,1,1,1,1,0,0,1,
 0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,
 1,1,0,1,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,0,1,1,0,1,0,
 0,0,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,1,1,0,1,
 1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
 0,1,0,0,1,1,1,1,0,0,1,1,0,0,0,1,0,1,1,1,0,1,1,0,1,1,1,0,0,1,
 1,0,0,0,1,0,1,1,0,1,0,0,0,1,1,1,1,0,1,1,1,0,1,0,0,1,0,1,1,0,
 0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,0,1,0,0,1,0,0,1,1,0,1,0,1,
 1,0,1,1,1,1,1,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,0,0,0,1,1,0,0,
 1,1,0,1,0,0,1,0,0,0,1,1,1,1,0,1,1,1,1,1,1,0,0,0,1,1,0,1,1,0,
 1,1,0,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,0,1,0,
 1,1,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,0,0,1,0,1,0,1,1,1,1,1,1,0,
 1,0,1,1,1,1,1,0,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,
 1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,0,1,0,0,1,1,1,1,
 1,1,1,1,1,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,0,0,1,1,0,0,
 1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,0,0,0,0,1,1,0,1,1,1,0,0,1,1,
 1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,0,0,0,1,0,0,0,1,1,0,
 1,1,0,1,1,1,1,1,0,1,1,0,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1 };
	int pOutBuffer[99];
	int n = FindPath(0, 0, 29, 29, pMap, 30, 30, pOutBuffer, 99);
	Vector2D *coords = new Vector2D[n];
	Vector2D prev = Vector2D(0, 0);
	Vector2D last_block = Vector2D(0, 0);
	coords = getAllTrueBlocks(n, pOutBuffer);
	int x_block = 0, y_block = 0, courseWidth = 1200, courseHeight = 1200,
		prev_x_block = 0, readCounter = 0, path_point = 0, bytes_read, mistakes = 0;
	float prev_x = 0, prev_y = 0, speed;
	std::cout << "Get Ready... \n";
	Sleep(2000);
	for (int n = 0; n < 3; n++) {
		std::cout << 3 - n << "\n";
		Sleep(1000);
	}
	std::cout << "GO! \n";
	clock_t start_read = clock();
	clock_t travel_read;
	bool complete = false;
	int steer = 0;
	while (!complete) {
		Sleep(50);
		pFile = fopen("C:\\dev\\output\\output.dat", "r");
		bytes_read = fread(in_arr, sizeof(float), sizeof(in_arr), pFile);
		Vector2D block = currentBlock(in_arr[0], in_arr[1]);
		Vector2D dir = Vector2D(in_arr[2], in_arr[3]);
		fclose(pFile);
		if ((block.get_x() == 29) && (block.get_y() == 29)) {
			complete = true;
		}
		if ((block.get_x() != last_block.get_x()) || (block.get_y() != last_block.get_y())) {
			last_block = block;
			if (pMap[getBufArrPos(block.get_x(), block.get_y())] == 0) {
				std::cout << "Ospie \n";
				mistakes++;
			}
		}
	}
	travel_read = (clock() - start_read);
	std::cout << "Total Course Traversal Time: " << (travel_read) << "ms \n";
	std::cout << "Obstacles Hit: " << mistakes << "\n";
	Sleep(60000);
}

int main() {
	maze();
	Sleep(60000);
	return 0;
}


/*
void PrintSolution(int n, int* pOutBuffer)
{
	if (n == -1)
	{
		std::std::cout << "no path found!" << std::endl;
		return;
	}

	std::std::cout << "n Elements : " << n << std::endl;
	for (int i = 0; i < n; ++i)
		std::std::cout << "move : " << pOutBuffer[i] << std::endl;
}
void Test5()
{
	unsigned char pMap[] =
	{
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1
	};
	int pOutBuffer[19];
	int n = FindPath(1, 1, 3, 3, pMap, 5, 5, pOutBuffer, 19);
	PrintSolution(n, pOutBuffer);
}

*/