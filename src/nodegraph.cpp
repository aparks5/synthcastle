#include "nodegraph.h"

std::shared_ptr<Node>& NodeGraph::node(const int id)
{
	return const_cast<std::shared_ptr<Node>&>(static_cast<const NodeGraph*>(this)->node(id));
}

const std::shared_ptr<Node>& NodeGraph::node(const int id) const
{
	const auto& iter = nodes_.find(id);
	assert(iter != nodes_.end());
	return *iter;
}

Span<const int> NodeGraph::neighbors(int node_id) const
{
	const auto iter = node_neighbors_.find(node_id);
	assert(iter != node_neighbors_.end());
	return *iter;
}

Span<const NodeGraph::Edge> NodeGraph::edges() const
{
	return edges_.elements();
}

size_t NodeGraph::num_edges_from_node(const int id) const
{
	auto iter = edges_from_node_.find(id);
	assert(iter != edges_from_node_.end());
	return *iter;
}

int NodeGraph::insert_node(const std::shared_ptr<Node>& node)
{
	const int id = current_id_++;
	printf("INSERTING NODE id (%d) type (%d)\n", id, node->type);
	assert(!nodes_.contains(id));
	nodes_.insert(id, node);
	edges_from_node_.insert(id, 0);
	node_neighbors_.insert(id, std::vector<int>());
	return id;
}

void NodeGraph::erase_node(const int id)
{

	// first, remove any potential dangling edges
	{
		static std::vector<int> edges_to_erase;

		for (const Edge& edge : edges_.elements())
		{
			if (edge.contains(id))
			{
				edges_to_erase.push_back(edge.id);
			}
		}

		for (const int edge_id : edges_to_erase)
		{
			erase_edge(edge_id);
		}

		edges_to_erase.clear();
	}

	nodes_.erase(id);
	edges_from_node_.erase(id);
	node_neighbors_.erase(id);
}

int NodeGraph::insert_edge(const int from, const int to)
{
	const int id = current_id_++;
	assert(!edges_.contains(id));
	assert(nodes_.contains(from));
	assert(nodes_.contains(to));
	edges_.insert(id, Edge(id, from, to));

	// update neighbor count
	assert(edges_from_node_.contains(from));
	*edges_from_node_.find(from) += 1;
	// update neighbor list
	assert(node_neighbors_.contains(from));
	node_neighbors_.find(from)->push_back(to);

	return id;
}

void NodeGraph::erase_edge(const int edge_id)
{
	// This is a bit lazy, we find the pointer here, but we refind it when we erase the edge based
	// on id key.
	assert(edges_.contains(edge_id));
	const Edge& edge = *edges_.find(edge_id);

	// update neighbor count
	assert(edges_from_node_.contains(edge.from));
	int& edge_count = *edges_from_node_.find(edge.from);
	assert(edge_count > 0);
	edge_count -= 1;

	// update neighbor list
	{
		assert(node_neighbors_.contains(edge.from));
		auto neighbors = node_neighbors_.find(edge.from);
		auto iter = std::find(neighbors->begin(), neighbors->end(), edge.to);
		assert(iter != neighbors->end());
		neighbors->erase(iter);
	}

	edges_.erase(edge_id);
}

