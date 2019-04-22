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

#include "OrthogonalJPS.h"
#include "PathfindingHeuristics.h"
#include <unordered_map> 

namespace PathFinding
{
	int OrthogonalJPS::GetPath(const MapCoord2i & start, const MapCoord2i & goal, const Map & map, int * pOutBuffer, const int nOutBufferSize) const
	{
		// edge cases
		if (nOutBufferSize == 0)
			return -1;

		if (start == goal)
		{
			pOutBuffer[0] = map.GetUnrolledIndexForCoord(goal);
			return 1;
		}

		// helper structures for tracking the search 
		std::unordered_map<int, MapCoord2i> prevPointTo;
		std::unordered_map<int, int> costToPoint;
		JumpPointQueue jumpPointsToVisitQueue;

		// initialization
		int startIndex = map.GetUnrolledIndexForCoord(start);
		int goalIndex = map.GetUnrolledIndexForCoord(goal);

		jumpPointsToVisitQueue.push({ 0, start });
		prevPointTo[startIndex] = start;
		costToPoint[startIndex] = 0;

		// JPS search
		bool searching = true;
		while (!jumpPointsToVisitQueue.empty() && searching)
		{
			// the current bifurcation point
			JumpPoint current = jumpPointsToVisitQueue.top();
			jumpPointsToVisitQueue.pop();

			// prev dir
			int currentPointIndex = map.GetUnrolledIndexForCoord(current.point);
			MapVec2i dirToPrev = GetNormalizedDir(current.point, prevPointTo[currentPointIndex]);

			// get jump points for this point in the ortoghonal directions
			for (auto & dir : N4Offsets)
			{
				// avoid checking on the same direction we were coming 
				if (dir == dirToPrev || (dirToPrev.x == 0 && dirToPrev.y == 0))
					continue;

				JumpPoint succesor;
				const bool SearchOnOrthogonalDirections = true;
				if (!SearchJumpPointAlongLine(current.point, dir, { -dir.y, dir.x }, map, goal, succesor.point, SearchOnOrthogonalDirections))
					continue;

				//early exit! if we have reached the goal
				if (succesor.point == goal)
				{
					int succesorPointIndex = map.GetUnrolledIndexForCoord(succesor.point);
					prevPointTo[succesorPointIndex] = current.point;
					searching = false;
					break;
				}

				// process successor as new Jump point with priority
				int succesorPointIndex = map.GetUnrolledIndexForCoord(succesor.point);

				// cost to succesor is computed from the manhatan distance
				int successorCost = costToPoint[currentPointIndex] + PathfindingHeuristics::ManhattanDistance(current.point, succesor.point);
				if (!costToPoint.count(succesorPointIndex) || successorCost < costToPoint[succesorPointIndex])
				{
					costToPoint[succesorPointIndex] = successorCost;
					prevPointTo[succesorPointIndex] = current.point;

					succesor.priority = successorCost + PathfindingHeuristics::ManhattanDistance(succesor.point, goal);
					
					jumpPointsToVisitQueue.push(std::move(succesor));
				}
			}
		}

		//see if we have found the path succesfully
		if (searching)
			return -1;

		//backtrack the path (doesnt include the start pos)
		MapCoord2i currentPoint = goal;
		int idx = nOutBufferSize;
		while (currentPoint != start && idx > 0)
		{
			idx--;
			int navIdx = map.GetUnrolledIndexForCoord(currentPoint);
			pOutBuffer[idx] = navIdx;

			// iterate over the line to prev point
			MapCoord2i prevpoint = prevPointTo[navIdx];
			MapVec2i dir = GetNormalizedDir(currentPoint, prevpoint);
			MapVec2i linePoint = currentPoint + dir;
			while (linePoint != prevpoint && idx > 0)
			{
				idx--;
				int navIdx = map.GetUnrolledIndexForCoord(linePoint);
				pOutBuffer[idx] = navIdx;
				linePoint = linePoint + dir;
			}
			currentPoint = prevpoint;
		}

		// the path was longer than the output buffer
		if (currentPoint != start && idx == 0)
			return -1;

		// set the path
		int nElements = nOutBufferSize - idx;
		std::memmove(pOutBuffer, pOutBuffer + idx, nElements * sizeof(int));

		return nElements;
	}

	bool OrthogonalJPS::SearchJumpPointAlongLine(const MapCoord2i & lineStart, const MapVec2i & dir, const MapVec2i & orthoDir, const Map & map, const MapCoord2i & goal, MapVec2i & foundJumpPoint, bool searchOnOrthogonalDir) const
	{
		// returns true if, along the line starting at lineStart with direction dir, we find a node with forced neighbours. 
		// false otherwise

	    // search (look for forced neighbours on the candidate point)
		MapCoord2i prevPointCandidate = lineStart; 
		MapCoord2i jumpPointCandidate = lineStart + dir; 
		while (true)
		{
			if (!map.IsGridUnitWalkable(jumpPointCandidate))
				return false;

			if (jumpPointCandidate == goal || HasForcedNeighbours(prevPointCandidate, jumpPointCandidate, orthoDir, map))
			{
				foundJumpPoint = jumpPointCandidate;
				return true;
			}

			if (searchOnOrthogonalDir)
			{
				if (SearchJumpPointAlongLine(jumpPointCandidate, orthoDir, dir, map, goal, foundJumpPoint) || 
					SearchJumpPointAlongLine(jumpPointCandidate, orthoDir * -1, dir, map, goal, foundJumpPoint))
				{
					foundJumpPoint = jumpPointCandidate;
					return true;
				}
			}

			// iterate
			prevPointCandidate = jumpPointCandidate; 
			jumpPointCandidate = jumpPointCandidate + dir;
		}

		return false;
	}

	bool OrthogonalJPS::HasForcedNeighbours(const MapCoord2i & prevPoint, const MapCoord2i & point, const MapVec2i & orthoDir, const Map & map) const
	{
		if (map.IsGridUnitWalkable(point + orthoDir) && !map.IsGridUnitWalkable(prevPoint + orthoDir))
			return true;

		else if (map.IsGridUnitWalkable(point - orthoDir) && !map.IsGridUnitWalkable(prevPoint - orthoDir))
			return true;

		return false;
	}

	MapVec2i OrthogonalJPS::GetNormalizedDir(const MapCoord2i & start, const MapCoord2i & end) const
	{
		MapVec2i vec = { 0, 0 };

		if (start.x == end.x)
			vec.y = (start.y > end.y ? -1 : 1);
		else
			vec.x = (start.x > end.x ? -1 : 1);

		return vec; // move semantics
	}

}