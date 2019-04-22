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

#ifndef ORTHO_JPS_PATH_FIND_H
#define ORTHO_JPS_PATH_FIND_H

#include "PathFinding.h"

namespace PathFinding
{
	typedef SearchPoint JumpPoint;

	// TODO: replace with custom queue implementation here ( must implement the pop(), top() and push() methods )
	typedef SearchPointQueue JumpPointQueue;

	// A modifed version of JPS that works when we can't move diagonally
	// Created by Ricardo David, New Zealand 2017
	// references : http://grastien.net/ban/articles/hg-aaai11.pdf , https://harablog.wordpress.com/category/pathfinding/
	class OrthogonalJPS : public PathFind
	{
	public:
		int GetPath(const MapCoord2i & start, const MapCoord2i & goal, const Map & map, int* pOutBuffer, const int nOutBufferSize) const override;

	private:
		bool SearchJumpPointAlongLine(const MapCoord2i & lineStart, const MapVec2i & dir, const MapVec2i & orthoDir, const Map & map, const MapCoord2i & goal, MapVec2i & foundJumpPoint, bool searchOnOrthogonalDir = false) const;
		bool HasForcedNeighbours(const MapCoord2i & prevPoint, const MapCoord2i & point, const MapVec2i & orthoDir, const Map & map) const;
		MapVec2i GetNormalizedDir(const MapCoord2i & start, const MapCoord2i & end) const;
	};

}


#endif // ORTHO_JPS_PATH_FIND_H
