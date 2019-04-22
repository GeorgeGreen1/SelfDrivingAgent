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

#ifndef PATH_FIND_H
#define PATH_FIND_H

#include "Map.h"
#include <vector>
#include <queue>
#include <functional>

namespace PathFinding
{
	struct SearchPoint {
		int priority;
		MapCoord2i point;

		bool operator <(const SearchPoint & other) const
		{
			return priority < other.priority;
		}

		bool operator >(const SearchPoint & other) const
		{
			return priority > other.priority;
		}
	};

	// TODO: replace with custom queue implementation here ( must implement the pop(), top() and push() methods )
	typedef std::priority_queue<SearchPoint, std::vector<SearchPoint>, std::greater<SearchPoint>> SearchPointQueue;

	// offsets used for navigating to neighbours
	const std::vector<MapCoord2i> N4Offsets = { MapCoord2i{ 1,0 }, MapCoord2i{ 0,1 }, MapCoord2i{ -1,0 }, MapCoord2i{ 0,-1 } };
	const std::vector<MapCoord2i> N8Offsets = { MapCoord2i{ 1,0 }, MapCoord2i{ 1,-1 }, MapCoord2i{ 0, -1 }, MapCoord2i{ -1,-1 } , MapCoord2i{ -1, 0 }, MapCoord2i{ -1,1 }, MapCoord2i{ 0,1 }, MapCoord2i{ 1,1 } };

	// generic path find interface class
	class PathFind
	{
	public:
		virtual ~PathFind() {};
		virtual int GetPath(const MapCoord2i & start, const MapCoord2i & goal, const Map & map, int* pOutBuffer, const int nOutBufferSize) const = 0;

	protected:
		PathFind() {};
	};

}


#endif // PATH_FIND_H
