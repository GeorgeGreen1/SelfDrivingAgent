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

#ifndef PATH_FIND_MAP_H
#define PATH_FIND_MAP_H

#include "MapVec2i.h"

namespace PathFinding
{
	struct Map
	{
		const unsigned char* pMap;
		int nMapWidth;
		int nMapHeight;

		inline bool IsCoordValid(const MapCoord2i & c) const
		{
			return c.x >= 0 && c.x < nMapWidth && c.y >= 0 && c.y < nMapHeight;
		}

		inline unsigned int GetUnrolledIndexForCoord(const MapCoord2i & c) const
		{
			return  c.x + c.y * nMapWidth;
		}

		inline bool IsGridUnitWalkable(const MapCoord2i & c) const
		{
			return IsCoordValid(c) && static_cast<int>(pMap[GetUnrolledIndexForCoord(c)]) == 1;
		}

		// callee is responsible of checking if the coord is valid or not
		inline bool IsGridUnitWalkable(int idx) const
		{
			return static_cast<int>(pMap[idx]) == 1;
		}
	};

}

#endif // !PATH_FIND_MAP_H