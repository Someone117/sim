/*
 *  CUDA based triangle mesh path tracer using BVH acceleration by Sam lapere,
 * 2016 BVH implementation based on real-time CUDA ray tracer by Thanassis
 * Tsiodras, http://users.softlab.ntua.gr/~ttsiod/cudarenderer-BVH.html
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __BVH_H_
#define __BVH_H_

#include "../Model/Models/Model.h"
#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <list>
#include <vector>
struct Triangle {
  // indexes in vertices array
  unsigned _idx1;
  unsigned _idx2;
  unsigned _idx3;
};

// The nice version of the BVH - a shallow hierarchy of inner and leaf nodes
struct BVHNode {
  glm::vec3 _bottom;
  glm::vec3 _top;
  virtual bool IsLeaf() = 0; // pure virtual
};

struct BVHInner : BVHNode {
  BVHNode *_left;
  BVHNode *_right;
  virtual bool IsLeaf() { return false; }
};

struct BVHLeaf : BVHNode {
  std::list<const Triangle *> _triangles;
  virtual bool IsLeaf() { return true; }
};

struct CacheFriendlyBVHNode {
  // bounding box
  glm::vec3 _bottom;
  glm::vec3 _top;

  // parameters for leafnodes and innernodes occupy same space (union) to save
  // memory top bit discriminates between leafnode and innernode no pointers,
  // but indices (int): faster

  union {
    // inner node - stores indexes to array of CacheFriendlyBVHNode
    struct {
      unsigned _idxLeft;
      unsigned _idxRight;
    } inner;
    // leaf node: stores triangle count and starting index in triangle list
    struct {
      unsigned _count; // Top-most bit set, leafnode if set, innernode otherwise
      unsigned _startIndexInTriIndexList;
    } leaf;
  } u;
};

struct Ray {
  glm::vec3 O, D;
  float t = INFINITY;
  glm::vec3 inv;
};

// The ugly, cache-friendly form of the BVH: 32 bytes
void CreateCFBVH(); // CacheFriendlyBVH

// The single-point entrance to the BVH - call only this
void UpdateBoundingVolumeHierarchy(const char *filename,
                                   Infinite::Model mainModel);

bool update();

extern std::vector<Ray> LIDAR;

void destroyBVH();

extern unsigned g_trianglesNo;
extern Triangle g_triangles[1290];
extern std::vector<Infinite::Vertex> vertices;

#endif