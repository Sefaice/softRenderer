#include "halfedge.h"

#include <iostream>
#include <utility>
#include <map>

unsigned int HalfEdge::h_Vertex::degree() const {
	unsigned int d = 0;
	HalfEdgeRef h = halfEdge;
	do {
		d++;
		h = h->twin->next;
	} while (h != halfEdge);

	return d;
}

unsigned int HalfEdge::h_Face::degree() const {
	unsigned int d = 0;
	HalfEdgeRef h = halfEdge;
	do {
		d++;
		h = h->next;
	} while (h != halfEdge);

	return d;
}

void HalfEdge::sanity_check() {
	for (HalfEdgeRef h = halfEdges.begin(); h != halfEdges.end(); ++h) {
		if (h->next == halfEdges.end()) {
			std::cout << "HALFEDGE NEXT NULL!" << std::endl;
		}
		if (h->twin == halfEdges.end()) {
			std::cout << "HALFEDGE TWIN NULL!" << std::endl;
		}
		if (h->vertex == vertices.end()) {
			std::cout << "HALFEDGE VERTEX NULL!" << std::endl;
		}
		if (h->edge == edges.end()) {
			std::cout << "HALFEDGE EDGE NULL!" << std::endl;
		}
		if (h->face == faces.end()) {
			std::cout << "HALFEDGE FACE NULL!" << std::endl;
		}
	}

	for (VertexRef v = vertices.begin(); v != vertices.end(); ++v) {
		if (v->halfEdge == halfEdges.end()) {
			std::cout << "VERTEX HALFEDGE NULL!" << std::endl;
		}
	}

	for (EdgeRef e = edges.begin(); e != edges.end(); ++e) {
		if (e->halfEdge == halfEdges.end()) {
			std::cout << "EDGE HALFEDGE NULL!" << std::endl;
		}
	}

	for (FaceRef f = faces.begin(); f != faces.end(); ++f) {
		if (f->halfEdge == halfEdges.end()) {
			std::cout << "FACE HALFEDGE NULL!" << std::endl;
		}
	}
}

/*
	if verts has v, return position in vector,
	else insert v at back and return position
*/
unsigned int vertsHas(std::vector<vec3>& verts, vec3& v) {
	for (unsigned int i = 0; i < verts.size(); i++) {
		if (v.x == verts[i].x && v.y == verts[i].y && v.z == verts[i].z) {
			return i;
		}
	}

	// insert v into verts
	verts.push_back(v);
	return verts.size() - 1;
}

void HalfEdge::from_mesh_float(float* vertices, unsigned int verticesNum) {

	std::vector<std::vector<unsigned int>> polys;
	std::vector<vec3> verts;

	for (unsigned int i = 0; i < verticesNum; i += 12) {
		vec3 v0v = vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
		vec3 v1v = vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
		vec3 v2v = vec3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);
		vec3 v3v = vec3(vertices[i + 9], vertices[i + 10], vertices[i + 11]);
		unsigned int v0 = vertsHas(verts, v0v);
		unsigned int v1 = vertsHas(verts, v1v);
		unsigned int v2 = vertsHas(verts, v2v);
		unsigned int v3 = vertsHas(verts, v3v);
		polys.push_back({ v0, v1, v2, v3 });
	}

	from_poly(polys, verts);

	sanity_check();
}

void HalfEdge::from_mesh(std::vector<vec3> vertsVector) {

	std::vector<std::vector<unsigned int>> polys;
	std::vector<vec3> verts;

	for (std::vector<vec3>::iterator i = vertsVector.begin(); i != vertsVector.end(); i += 4) {
		unsigned int v0 = vertsHas(verts, *i);
		unsigned int v1 = vertsHas(verts, *(i + 1));
		unsigned int v2 = vertsHas(verts, *(i + 2));
		unsigned int v3 = vertsHas(verts, *(i + 3));
		polys.push_back({ v0, v1, v2, v3 });
	}

	from_poly(polys, verts);

	sanity_check();
}

void HalfEdge::to_mesh_EBO(std::vector<std::vector<unsigned int>>& triangles, std::vector<vec3>& verts) {

	triangles.clear();
	verts.clear();
	std::map<unsigned int, unsigned int> layout; // vertex id - corresponding verts index

	unsigned int idx = 0;
	for (VertexRef v = vertices.begin(); v != vertices.end(); ++v, ++idx) {
		verts.push_back(v->pos);
		layout[v->id] = idx;
	}

	for (FaceRef f = faces.begin(); f != faces.end(); ++f) {
		HalfEdgeRef h = f->halfEdge;
		unsigned int i = layout[h->vertex->id];
		h = h->next;
		unsigned int j = layout[h->vertex->id];
		h = h->next;
		unsigned int k = layout[h->vertex->id];
		h = h->next;
		if (h != f->halfEdge) { // quad
			unsigned int l = layout[h->vertex->id];
			triangles.push_back({i, j, k, l});
			// triangles.push_back({i, k, l});
		}
		else { // triangle
			triangles.push_back({ i, j, k });
		}
	}
}

void HalfEdge::to_mesh(std::vector<std::pair<vec3, vec3>>& verticesVector, std::vector<vec3>& verticesVector_quad){

	verticesVector.clear();
	verticesVector_quad.clear();

	for (FaceRef f = faces.begin(); f != faces.end(); ++f) {
		HalfEdgeRef h = f->halfEdge;
		vec3 i = h->vertex->pos;
		h = h->next;
		vec3 j = h->vertex->pos;
		h = h->next;
		vec3 k = h->vertex->pos;
		h = h->next;
		if (h != f->halfEdge) { // quad
			vec3 l = h->vertex->pos;
			vec3 norm1 = cross(j - i, k - i);
			norm1 = normalize(norm1);

			verticesVector.push_back(std::make_pair(i, norm1));
			verticesVector.push_back(std::make_pair(j, norm1));
			verticesVector.push_back(std::make_pair(k, norm1));

			verticesVector.push_back(std::make_pair(i, norm1));
			verticesVector.push_back(std::make_pair(k, norm1));
			verticesVector.push_back(std::make_pair(l, norm1));

			verticesVector_quad.push_back(i);
			verticesVector_quad.push_back(j);
			verticesVector_quad.push_back(k);
			verticesVector_quad.push_back(l);
		}
		else { // triangle
			std::cout << "TRIANGLE TO_MESH NOT SUPPORTED!" << std::endl;
		}
	}
}

void HalfEdge::from_poly(const std::vector<std::vector<unsigned int>>& polygons, const std::vector<vec3>& verts) {

	clear();

	//for (auto i = verts.begin(); i != verts.end(); i++) {
	//	std::cout << i->x << " " << i->y << " " << i->z<< std::endl;
	//}

	// vertex to h_vertex map
	std::map<unsigned int, VertexRef> indexToVertex;
	// ordered vertices to halfedge map
	std::map<std::pair<unsigned int, unsigned int>, HalfEdgeRef> pairToHalfedge;

	// iter polygons
	for (std::vector<std::vector<unsigned int>>::const_iterator i = polygons.begin(); i != polygons.end(); ++i) {

		FaceRef f = new_face();

		// iter vertices of a polygon
		HalfEdgeRef oldh; // for halfedge twin
		for (std::vector<unsigned int>::const_iterator j = i->begin(); (j + 1) != i->end(); ++j) {

			HalfEdgeRef h = new_halfEdge();
			h->next = halfEdges.end();
			h->twin = halfEdges.end();

			// vertex
			if (indexToVertex.find(*j) != indexToVertex.end()) { // vertex already been inited
				// halfedge veretx
				h->vertex = indexToVertex[*j];
			} else {
				VertexRef v = new_vertex();
				indexToVertex[*j] = v;
				v->pos = verts[*j];
				v->halfEdge = h;
				h->vertex = v;
			}

			// edge
			std::pair<unsigned int, unsigned int> ab = std::make_pair(*j, *(j + 1));
			std::pair<unsigned int, unsigned int> ba = std::make_pair(*(j + 1), *j);
			pairToHalfedge[ab] = h;
			if (pairToHalfedge.find(ba) != pairToHalfedge.end()) { // edge already been inited
				// halfedge twin
				h->twin = pairToHalfedge[ba];
				pairToHalfedge[ba]->twin = h;
				// halfedge edge
				h->edge = pairToHalfedge[ba]->edge;
			} else {
				EdgeRef e = new_edge();
				e->halfEdge = h;
				h->edge = e;
			}
			
			// halfedge next
			if (j != i->begin()) {
				oldh->next = h;
			}
			// halfedge face
			h->face = f;

			oldh = h;
		}

		// process last vertex
		unsigned int a = i->back();
		unsigned int b = i->front();
		unsigned int c = *std::next(i->begin(), 1);
		HalfEdgeRef h = new_halfEdge();
		h->next = pairToHalfedge[std::make_pair(b, c)];
		h->twin = halfEdges.end();
		// vertex
		if (indexToVertex.find(a) != indexToVertex.end()) { // vertex already been inited
			h->vertex = indexToVertex[a];
		}
		else {
			VertexRef v = new_vertex();
			indexToVertex[a] = v;
			v->pos = verts[a];
			v->halfEdge = h;
			h->vertex = v;
		}
		// edge
		std::pair<unsigned int, unsigned int> ab = std::make_pair(a, b);
		std::pair<unsigned int, unsigned int> ba = std::make_pair(b, a);
		pairToHalfedge[ab] = h;
		if (pairToHalfedge.find(ba) != pairToHalfedge.end()) {
			h->twin = pairToHalfedge[ba];
			pairToHalfedge[ba]->twin = h;
			h->edge = pairToHalfedge[ba]->edge;
		}
		else {
			EdgeRef e = new_edge();
			e->halfEdge = h;
			h->edge = e;
		}
		// halfedge
		oldh->next = h;
		h->face = f;
		
		f->halfEdge = h;
	}

	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << vertices.size() << " " << indexToVertex.size() << std::endl; // 8
	//std::cout << edges.size() << std::endl; // 12
	//std::cout << faces.size() << std::endl; // 6
	//std::cout << halfEdges.size() << " " << pairToHalfedge.size() << std::endl; // 24
}

void HalfEdge::subdivide() {

	//linear_subdivide_positions();
	catmullclark_subdivide_positions();

	std::vector<std::vector<unsigned int>> polys;
	std::vector<vec3> verts;
	std::map<unsigned int, unsigned int> layout; // vertex/edge/face id - corresponding verts index

	// build subdivided polys and vertices

	unsigned int idx = 0;

	for (VertexRef v = vertices.begin(); v != vertices.end(); ++v, ++idx) {
		verts.push_back(v->new_pos);
		layout[v->id] = idx;
	}
	for (EdgeRef e = edges.begin(); e != edges.end(); ++e, ++idx) {
		verts.push_back(e->new_pos);
		layout[e->id] = idx;
	}
	for (FaceRef f = faces.begin(); f != faces.end(); ++f, ++idx) {
		verts.push_back(f->new_pos);
		layout[f->id] = idx;
	}

	for (FaceRef f = faces.begin(); f != faces.end(); ++f) {
		unsigned int i = layout[f->id];
		HalfEdgeRef h = f->halfEdge;
		do {
			unsigned int j = layout[h->edge->id];
			unsigned int k = layout[h->next->vertex->id];
			unsigned int l = layout[h->next->edge->id];
			polys.push_back({ i, j, k, l });
			h = h->next;
		} while (h != f->halfEdge);
	}

	//

	from_poly(polys, verts);

	sanity_check();
}

void HalfEdge::linear_subdivide_positions() {
	
	// For each vertex, assign Vertex::new_pos to
	// its original position, Vertex::pos.
	for (VertexRef v = vertices.begin(); v != vertices.end(); ++v) {
		v->new_pos = v->pos;
	}

	// For each edge, assign the midpoint of the two original
	// positions to Edge::new_pos.
	for (EdgeRef e = edges.begin(); e != edges.end(); ++e) {
		VertexRef v0 = e->halfEdge->vertex;
		VertexRef v1 = e->halfEdge->twin->vertex;

		e->new_pos = 0.5 * v0->pos + 0.5 * v1->pos;
	}

	// For each face, assign the centroid (i.e., arithmetic mean)
	// of the original vertex positions to Face::new_pos. Note
	// that in general, NOT all faces will be triangles!
	for (FaceRef f = faces.begin(); f != faces.end(); ++f) {
		// get all boundary vertices
		std::vector<VertexRef> bVertices;
		HalfEdgeRef h = f->halfEdge;
		do {
			bVertices.push_back(h->vertex);
			h = h->next;
		} while (h != f->halfEdge);

		vec3 sumPos;
		for (std::vector<VertexRef>::iterator i = bVertices.begin(); i != bVertices.end(); ++i) {
			sumPos = sumPos + (*i)->pos;
		}

		f->new_pos = sumPos / (float)bVertices.size();
	}

}

void HalfEdge::catmullclark_subdivide_positions() {
	// The implementation for this routine should be
	// a lot like Halfedge_Mesh:linear_subdivide_positions:(),
	// except that the calculation of the positions themsevles is
	// slightly more involved, using the Catmull-Clark subdivision
	// rules. (These rules are outlined in the Developer Manual.)

	// Faces
	for (FaceRef f = faces.begin(); f != faces.end(); ++f) {
		// get all boundary vertices
		std::vector<VertexRef> bVertices;
		HalfEdgeRef h = f->halfEdge;
		do {
			bVertices.push_back(h->vertex);
			h = h->next;
		} while (h != f->halfEdge);

		vec3 sumPos;
		for (std::vector<VertexRef>::iterator i = bVertices.begin(); i != bVertices.end(); ++i) {
			sumPos = sumPos + (*i)->pos;
		}

		f->new_pos = sumPos / (float)bVertices.size();
	}

	// Edges
	for (EdgeRef e = edges.begin(); e != edges.end(); ++e) {
		VertexRef v0 = e->halfEdge->vertex;
		VertexRef v1 = e->halfEdge->twin->vertex;
		vec3 v2_pos = e->halfEdge->face->new_pos;
		vec3 v3_pos = e->halfEdge->twin->face->new_pos;

		e->new_pos = (v0->pos + v1->pos + v2_pos + v3_pos) / 4.0f;
	}

	// Vertices
	for (VertexRef v = vertices.begin(); v != vertices.end(); ++v) {

		unsigned int n = v->degree();

		// Q

		std::vector<FaceRef> bFaces;
		HalfEdgeRef h = v->halfEdge->twin;
		do {
			bFaces.push_back(h->face);
			h = h->next->twin;
		} while (h != v->halfEdge->twin);

		vec3 Q;
		for (std::vector<FaceRef>::iterator i = bFaces.begin(); i != bFaces.end(); ++i) {
			Q = Q + (*i)->new_pos;
		}

		Q = Q / (float)bFaces.size();

		// R

		std::vector<EdgeRef> bEdges;
		HalfEdgeRef hr = v->halfEdge->twin;
		do {
			bEdges.push_back(hr->edge);
			hr = hr->next->twin;
		} while (hr != v->halfEdge->twin);

		vec3 R;
		for (std::vector<EdgeRef>::iterator i = bEdges.begin(); i != bEdges.end(); ++i) {
			VertexRef v0 = (*i)->halfEdge->vertex;
			VertexRef v1 = (*i)->halfEdge->twin->vertex;
			R = R + (v0->pos + v1->pos) / 2.0f;
		}

		R = R / (float)bEdges.size();

		// S

		vec3 S = v->pos;

		v->new_pos = (Q + 2.0f * R + (float)(n - 3) * S) / (float)n;
	}
}