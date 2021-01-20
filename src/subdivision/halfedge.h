#include "../math.h"

#include <list>
#include <vector>
#include <tuple>

class HalfEdge {
public:

	class h_Vertex;
	class h_Edge;
	class h_Face;
	class h_HalfEdge;

	using VertexRef = std::list<h_Vertex>::iterator;
	using EdgeRef = std::list<h_Edge>::iterator;
	using FaceRef = std::list<h_Face>::iterator;
	using HalfEdgeRef = std::list<h_HalfEdge>::iterator;

	class h_Vertex {
	public:
		h_Vertex(unsigned int _id) : id(_id) {}
		// Returns the number of edges incident from this vertex
		unsigned int degree() const;

		vec3 pos;
		vec3 new_pos;
		unsigned int id;
		HalfEdgeRef halfEdge;
	};

	class h_Edge {
	public:
		h_Edge(unsigned int _id) : id(_id) {}
		vec3 new_pos;
		unsigned int id;
		HalfEdgeRef halfEdge;
	};

	class h_Face {
	public:
		h_Face(unsigned int _id) : id(_id) {}
		// Returns the number of vertices/edges in this face
		unsigned int degree() const;
		
		vec3 new_pos;
		unsigned int id;
		HalfEdgeRef halfEdge;
	};

	class h_HalfEdge {
	public:
		h_HalfEdge(unsigned int _id) : id(_id) {}
		
		unsigned int id;
		HalfEdgeRef next;
		HalfEdgeRef twin;
		VertexRef vertex;
		EdgeRef edge;
		FaceRef face;
	};

	VertexRef new_vertex() { return vertices.insert(vertices.end(), h_Vertex(gloabl_index++)); }
	EdgeRef new_edge() { return edges.insert(edges.end(), h_Edge(gloabl_index++)); }
	FaceRef new_face() { return faces.insert(faces.end(), h_Face(gloabl_index++)); }
	HalfEdgeRef new_halfEdge() { return halfEdges.insert(halfEdges.end(), h_HalfEdge(gloabl_index++)); }

	void clear() { vertices.clear(); edges.clear(); faces.clear(); halfEdges.clear(); }
	
	void sanity_check();

	// convert quad mesh to poly indices and vertices
	void from_mesh_float(float* vertices, unsigned int verticesNum);
	void from_mesh(std::vector<vec3> vertsVector);

	// return VAO and EBO triangle mesh
	void to_mesh_EBO(std::vector<std::vector<unsigned int>>& triangles, std::vector<vec3>& verts);
	
	// return triangle mesh(only VAO, no EBO) for drawing and quad mesh for next subdivision
	void to_mesh(std::vector<std::pair<vec3, vec3>>& verticesVector, std::vector<vec3>& verticesVector_quad);

	// build halfedge mesh from poly indices and vertices
	void from_poly(const std::vector<std::vector<unsigned int>>& polygons, const std::vector<vec3>& verts);

	void subdivide();
	void linear_subdivide_positions();
	void catmullclark_subdivide_positions();

private:
	std::list<h_Vertex> vertices;
	std::list<h_Edge> edges;
	std::list<h_Face> faces;
	std::list<h_HalfEdge> halfEdges;

	unsigned int gloabl_index = 0;
};