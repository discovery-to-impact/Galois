#ifndef EMBEDDING_LIST_H
#define EMBEDDING_LIST_H
#include "edge.h"
#include "egonet.h"
#include "base_embedding.h"
#include "vertex_embedding.h"

template <typename ElementType, typename EmbeddingType,
	bool is_single=true, bool use_ccode=true, 
	bool shrink=false, bool use_formula=false>
class EmbeddingList {
public:
	EmbeddingList() {}
	~EmbeddingList() {}
	void allocate(Graph *graph, unsigned max_size, unsigned max_degree);
	void init_vertex(const VertexId vid);
	void construct_local_graph_from_vertex(const VertexId vid);
	void init_edge(const SEdge &edge);
	void construct_local_graph_from_edge(const SEdge &edge);

	void update_labels(unsigned level, VertexId src) {
		for (auto e : global_graph->edges(src)) {
			auto dst = global_graph->getEdgeDst(e);
			labels[dst] += 1 << level;
		}
	}
	void clear_labels(const VertexId src) {
		for (auto e : global_graph->edges(src)) {
			auto dst = global_graph->getEdgeDst(e);
			labels[dst] = 0; // mark the neighbors of edge.src
		}
	}
	void reset_labels(unsigned level)	{
		for (size_t emb_id = 0; emb_id < size(level+1); emb_id ++) {//restoring labels
			auto src = get_vertex(level+1, emb_id);
			labels[src] = level;
		}
	}
	void resume_labels(unsigned level, const VertexId src) {
		for (auto e : global_graph->edges(src)) {
			auto dst = global_graph->getEdgeDst(e);
			labels[dst] -= 1 << level;
		}
	}
	void reset_ids(const VertexId vid) {
		for (auto e : global_graph->edges(vid)) {
			auto dst = global_graph->getEdgeDst(e);
			ids[dst] = (unsigned)-1;
		}
	}
	void mark_neighbors() {
		VertexId src = get_vid(0, 0);
		VertexId dst = get_vid(1, 0);
		for (auto e : global_graph->edges(src)) {
			auto w = global_graph->getEdgeDst(e);
			if (dst == w) continue;
			labels[w] = 1;
		}
	}
	void triangles_and_wedges() {
		VertexId src = get_vid(0, 0);
		VertexId dst = get_vid(1, 0);
		for (auto e : global_graph->edges(dst)) {
			auto w = global_graph->getEdgeDst(e);
			if (w == src) continue;
			if (labels[w] == 1) {
				labels[w] = 3;
				T_vu[tri_count] = w;
				tri_count++;
			}
			else {
				W_u[wed_count] = w;
				wed_count++;
				labels[w] = 2;
			}
		}
	}
	void cycle() {
		for (Ulong j = 0; j < wed_count; j++) {
			auto src = W_u[j];
			for (auto e : global_graph->edges(src)) {
				auto dst = global_graph->getEdgeDst(e);
				if (labels[dst] == 1) cycle4_count ++;
			}
			W_u[j] = 0;
		}
	}
	void clique() {
		for (Ulong tr_i = 0; tr_i < tri_count; tr_i++) {
			auto src = T_vu[tr_i];
			for (auto e : global_graph->edges(src)) {
				auto dst = global_graph->getEdgeDst(e);
				if (labels[dst] == 3) clique4_count ++;
			}
			labels[src] = 0;
			T_vu[tr_i] = 0;
		}
	}
	inline void get_embedding(unsigned level, EmbeddingType &emb) {
		for (unsigned l = 0; l < level+1; l ++) {
			ElementType ele(history[l]);
			emb.set_element(l, ele);
		}
	}
	inline void get_embedding(unsigned level, unsigned pos, EmbeddingType &emb) {
		//std::cout << ", get_embedding: level = " << level << ", pos = " << pos;
		for (unsigned l = 0; l < level+1; l ++) {
			ElementType ele(history[l]);
			emb.set_element(level, ele);
		}
	}
	/*
	bool is_connected (unsigned level, VertexId vid, unsigned element_id) const {
		if (level == 1) return (get_label(vid) == element_id + 1) || (get_label(vid) == 3);
		else if (level == 2) {
			return (get_label(vid) & (1 << element_id));
		} else return false;
	}
	//*/

	size_t size() const { return sizes[cur_level]; }
	size_t size(unsigned level) const { return sizes[level]; }
	VertexId get_vertex(unsigned level, size_t i) const { return vid_lists[level][i]; }
	VertexId get_vid(unsigned level, size_t i) const { return vid_lists[level][i]; }
	std::vector<VertexId> get_history() const { return history; }
	VertexId get_history(unsigned level) const { return history[level]; }
	IndexTy get_idx(unsigned level, IndexTy id) const { return idx_lists[level][id]; }
	BYTE get_pid(unsigned level, size_t i) const { return pid_lists[level][i]; }
	BYTE get_src(unsigned level, size_t i) const { return src_indices[level][i]; }
	BYTE get_label(VertexId vid) const { return labels[vid]; }
	//unsigned get_label(VertexId vid) const { return labels[vid]; }
	unsigned get_level() const { return cur_level; }
	void set_size(unsigned level, size_t size) { sizes[level] = size; }
	void set_vid(unsigned level, size_t id, VertexId vid) { vid_lists[level][id] = vid; }
	void set_idx(unsigned level, size_t id, IndexTy idx) { idx_lists[level][id] = idx; }
	void set_pid(unsigned level, size_t id, BYTE pid) { pid_lists[level][id] = pid; }
	void set_src(unsigned level, size_t id, BYTE src) { src_indices[level][id] = src; }
	void set_label(VertexId vid, BYTE value) { labels[vid] = value; }
	//void set_label(VertexId vid, unsigned value) { labels[vid] = value; }
	void set_level(unsigned level) { cur_level = level; }
	Ulong get_tri_count() { return tri_count; }
	Ulong get_wed_count() { return wed_count; }
	Ulong get_cycle4_count() { return cycle4_count; }
	Ulong get_clique4_count() { return clique4_count; }
	void push_history(VertexId vid) { history.push_back(vid); }
	void pop_history() { history.pop_back(); }
	void inc_tri_count() { tri_count ++; }
	void inc_wed_count() { wed_count ++; }
	void inc_cycle4_count() { cycle4_count ++; }
	void inc_clique4_count() { clique4_count ++; }

	// egonet operations
	void init_egonet_degree(unsigned level, VertexId dst) {
		shrink_graph.set_degree(level, dst, 0);
	}
	void update_egonet(unsigned level);

	inline auto getEdgeDst(VertexId vid) const {
		if constexpr (shrink) {
			return shrink_graph.getEdgeDst(vid);
		} else { 
			return global_graph->getEdgeDst(vid);
		}
	}

	inline auto edge_begin(unsigned level, VertexId vid) {
		if constexpr (shrink) {
			return shrink_graph.edge_begin(vid);
		} else { 
			return global_graph->edge_begin(vid); // TODO: maybe incorrect
		}
	}

	inline auto edge_end(unsigned level, VertexId vid) {
		if constexpr (shrink) {
			return shrink_graph.edge_begin(vid) + shrink_graph.get_degree(level, vid);
		} else {
			return global_graph->edge_begin(vid); // TODO: maybe incorrect
		}
	}


protected:
	unsigned length;
	unsigned max_level;
	unsigned cur_level;

	UintList sizes;        // sizes[level]: no. of embeddings (i.e. no. of vertices in the the current level)
	ByteList labels;       // labels[i] is the label of each vertex i; it is the perfect hash table for checking in O(1) time if edge (triangle, etc) exists
	ByteLists pid_lists;   // pid[i] is the pattern id of each embedding
	IndexLists idx_lists;  // list of indices
	VertexLists vid_lists; // list of vertex ID
	ByteLists src_indices; // list of source indices
	std::vector<VertexId> history; 

	UintList ids;
	UintList old_ids;

	Graph *global_graph; // original input graph
	Graph *local_graph; // shrinking graph 
	Egonet shrink_graph;

	UintList T_vu; // T_vu is an array containing all the third vertex of each triangle
	UintList W_u;
	Ulong tri_count; // number of triangles incident to this edge
	Ulong wed_count; // number of wedges incident to this edge
	Ulong clique4_count;
	Ulong cycle4_count;
};

#endif
