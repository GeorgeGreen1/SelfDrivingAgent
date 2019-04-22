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

#include "AStar.h"
#include "PathfindingHeuristics.h"
#include <unordered_map> 

namespace PathFinding
{

	int AStar::GetPath(const MapCoord2i & start, const MapCoord2i & goal, const Map & map, int* pOutBuffer, const int nOutBufferSize) const
	{
		std::unordered_map<int, MapCoord2i> prevPointTo;
		std::unordered_map<int, int> costToPoint;
		SearchPointQueue searchPoints;

		// initialization
		int startIndex = map.GetUnrolledIndexForCoord(start);
		int goalIndex = map.GetUnrolledIndexForCoord(goal);

		searchPoints.push({ 0, start });
		prevPointTo[startIndex] = start;
		costToPoint[startIndex] = 0;

		// A* search
		while (!searchPoints.empty()) 
		{
			SearchPoint current = searchPoints.top();
			searchPoints.pop();

			//early exit! if we have reached the goal
			if (current.point == goal) 
				break;

			// iterate through neighbours
			// TODO: make N4 offsets an argument to this method, so this algorithm can be used with N8 offsets as well
			for (unsigned int k = 0; k < N4Offsets.size(); ++k) 
			{
				MapCoord2i nPoint = current.point + N4Offsets[k];

				// skip points out of the image range and on where we cannot move
				if (!map.IsGridUnitWalkable(nPoint))
					continue;

				int currentPointIndex = map.GetUnrolledIndexForCoord(current.point);
				int nPointIndex = map.GetUnrolledIndexForCoord(nPoint);
				
				int newCost = costToPoint[currentPointIndex] + 1; // 1 = assuming constant graph cost of moving to any grid unit 
				if (!costToPoint.count(nPointIndex) || newCost < costToPoint[nPointIndex]) 
				{
					costToPoint[nPointIndex] = newCost;
					int priority = newCost + PathfindingHeuristics::ManhattanDistance(nPoint, goal);
					searchPoints.push({ priority, nPoint });
					prevPointTo[nPointIndex] = current.point;
				}
			}
		}

		// see if we have found the path succesfully
		if (prevPointTo.find(goalIndex) == prevPointTo.end())
		{
			return -1;
		}

		// backtrack the path (doesnt include the start pos)
		MapCoord2i navCurrent = goal;
		int idx = nOutBufferSize;
		while (navCurrent != start && idx > 0)
		{
			idx--;
			int navIdx = map.GetUnrolledIndexForCoord(navCurrent);
			pOutBuffer[idx] = navIdx;
			navCurrent = prevPointTo[navIdx];
		}

		// the path was longer than the output buffer
		if (navCurrent != start && idx == 0)
			return -1;

		// set the path
		int nElements = nOutBufferSize - idx;
		std::memmove(pOutBuffer, pOutBuffer + idx, nElements * sizeof(int));

		return nElements;
	}

}