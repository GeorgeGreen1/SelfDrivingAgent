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


Vector2D getMidCoords(float x, float y) {
	return Vector2D((x * 40) + 20, (y * 40) + 20);
}

bool speedControl(float curr_x, float curr_y, float prev_x, float prev_y, bool going) {
	float speed = sqrt(pow((curr_x - prev_x), 2) + pow((curr_y - prev_y), 2)) / 1000 * 36000;
	bool goes = going;
	if ((speed < 50) && (!going)) {
		system("C:\\Users\\Jake\\source\\repos\\DriverAgent\\Debug\\greatqb.bat 1");
		goes = true;
	}
	if ((speed > 52) && (going)) {
		system("C:\\Users\\Jake\\source\\repos\\DriverAgent\\Debug\\greatqb.bat 0");
		goes = false;
	}
	return goes;
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

	/*PathFinding::AStar astar;
	return astar.GetPath({ nStartX, nStartY }, { nTargetX, nTargetY }, { pMap, nMapWidth, nMapHeight }, pOutBuffer, nOutBufferSize);*/
}


int main() {
	string line;
	FILE * pFile;
	float in_arr[4];
	vector<string> sep;
	unsigned char pMap[] =
	{ 1,0,0,1,1,1,1,0,1,0,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,
	 1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,1,1,0,1,0,1,1,0,1,1,1,1,0,0,1,
	 1,0,0,0,1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,
	 1,1,1,0,0,1,0,1,1,1,0,0,0,1,1,1,1,0,0,1,0,1,0,1,0,1,0,0,1,1,
	 1,0,1,1,0,1,1,0,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,
	 1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,0,0,1,1,1,0,0,1,0,1,1,1,
	 1,0,1,0,0,0,1,0,1,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,0,1,
	 1,1,1,0,1,1,0,0,0,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,0,1,1,1,
	 1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,0,1,1,0,0,0,
	 1,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1,
	 1,1,1,0,1,1,1,0,0,0,1,0,1,0,0,1,1,1,0,1,1,0,0,0,0,1,1,1,0,0,
	 1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0,1,1,1,1,
	 0,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,0,1,1,1,1,1,
	 1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,
	 1,1,1,1,1,1,1,1,1,0,1,0,1,0,0,1,1,0,1,1,0,1,0,1,1,1,0,1,1,1,
	 0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,1,1,
	 1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,
	 1,1,1,1,1,1,0,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,
	 1,0,1,1,0,0,1,1,1,1,1,1,0,0,0,1,1,0,1,0,1,0,1,0,1,1,1,0,0,0,
	 0,1,0,0,1,1,1,1,0,1,1,0,1,0,1,1,0,1,1,1,0,0,1,0,1,1,1,1,0,0,
	 1,1,0,0,1,1,1,1,0,0,1,1,1,0,1,0,0,1,1,0,0,1,1,1,0,1,1,0,1,0,
	 1,1,1,1,1,1,1,1,1,0,1,0,0,0,1,1,1,1,0,1,0,1,0,1,1,1,0,0,1,1,
	 1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,1,0,1,1,0,1,1,0,1,
	 1,1,1,0,1,1,0,0,0,1,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,1,
	 1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,
	 1,1,0,1,1,0,1,0,1,1,0,1,0,0,0,1,1,1,1,1,1,0,1,1,1,0,1,1,0,1,
	 0,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,
	 1,1,1,1,1,1,1,1,0,1,1,0,1,0,1,1,1,1,1,0,0,1,0,1,1,0,1,1,1,1,
	 1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,
	 1,1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,1 };
	int pOutBuffer[99];
	int n = FindPath(1, 1, 29, 29, pMap, 30, 30, pOutBuffer, 99);
	Vector2D *coords = new Vector2D[n];
	coords = getAllTrueBlocks(n, pOutBuffer);
	int x_block = 0, y_block = 0, courseWidth = 1200, courseHeight = 1200,
		prev_x_block = 0, readCounter = 0, path_point = 0;
	float prev_x = 0, prev_y = 0, speed;
	clock_t last_read = clock();
	bool going = false, reached_dest= false;
	while (true) {
		Sleep(50);
		pFile = fopen("C:\\dev\\output\\output.dat", "r");
		int bytes_read = fread(in_arr, sizeof(float), sizeof(in_arr), pFile);
		Vector2D block = currentBlock(in_arr[0], in_arr[1]);
		Vector2D dir = Vector2D(in_arr[2], in_arr[3]);
		fclose(pFile);
		Vector2D realCoords = getMidCoords(coords[path_point].get_x(), coords[path_point].get_y());
		Vector2D targetDist = realCoords - mapCoords(in_arr[0], in_arr[1]);
		targetDist.normalize();
		dir.normalize();
		//cout << targetDist.get_x() << ", " << targetDist.get_y() << "\n";
		float diffAngle = dir.angleTo(targetDist);
		cout << diffAngle << "\n";
		/*if (diffAngle > 0.1) {
			if (dir.get_x() < targetDist.get_x()) {
				cout << "LEFT \n";
			}
			else {
				cout << "RIGHT \n";
			}
		}*/
		if ((block.get_x() == coords[path_point].get_x()) && (block.get_y() == coords[path_point].get_y())) {
			path_point++;
			/*Vector2D realCoords = getMidCoords(coords[path_point].get_x(), coords[path_point].get_y());
			Vector2D targetDist = realCoords - mapCoords(in_arr[0], in_arr[1]);
			//cout << "Pthalo white, Alizzeran Crimson: " << targetDist.get_x() << ", " << targetDist.get_y() << "\n";
			cout << dir.angleBetween(targetDist) << "\n";*/
		}
		/*if (readCounter == 2) {
			going = speedControl(in_arr[0], in_arr[1], prev_x, prev_y, going);
			prev_x = in_arr[0];
			prev_y = in_arr[1];
			readCounter = 0;
		}
		readCounter++;
		last_read = clock();*/
	}
	return 0;
}


/*
void PrintSolution(int n, int* pOutBuffer)
{
	if (n == -1)
	{
		std::cout << "no path found!" << std::endl;
		return;
	}

	std::cout << "n Elements : " << n << std::endl;
	for (int i = 0; i < n; ++i)
		std::cout << "move : " << pOutBuffer[i] << std::endl;
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