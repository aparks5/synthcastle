#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <stack>
#include <stddef.h>
#include <utility>
#include <vector>

#include "node.h"
#include <memory>
#include <mutex>

template<typename ElementType>
struct Span
{
	using iterator = ElementType*;

	template<typename Container>
	Span(Container& c) : begin_(c.data()), end_(begin_ + c.size())
	{
	}

	iterator begin() const { return begin_; }
	iterator end() const { return end_; }

private:
	iterator begin_;
	iterator end_;
};

template<typename ElementType>
class IdMap
{
public:
	using iterator = typename std::vector<ElementType>::iterator;
	using const_iterator = typename std::vector<ElementType>::const_iterator;

	// Iterators

	const_iterator begin() const { return elements_.begin(); }
	const_iterator end() const { return elements_.end(); }

	// Element access

	Span<const ElementType> elements() const { return elements_; }

	// Capacity

	bool   empty() const { return sorted_ids_.empty(); }
	size_t size() const { return sorted_ids_.size(); }

	// Modifiers

	std::pair<iterator, bool> insert(int id, const ElementType& element);
	std::pair<iterator, bool> insert(int id, ElementType&& element);
	size_t                    erase(int id);
	void                      clear();

	// Lookup

	iterator       find(int id);
	const_iterator find(int id) const;
	bool           contains(int id) const;

	std::vector<ElementType> elements_;
	std::vector<int>         sorted_ids_;
};

template<typename ElementType>
std::pair<typename IdMap<ElementType>::iterator, bool> IdMap<ElementType>::insert(
	const int          id,
	const ElementType& element)
{
	auto lower_bound = std::lower_bound(sorted_ids_.begin(), sorted_ids_.end(), id);

	if (lower_bound != sorted_ids_.end() && id == *lower_bound)
	{
		return std::make_pair(
			std::next(elements_.begin(), std::distance(sorted_ids_.begin(), lower_bound)), false);
	}

	auto insert_element_at =
		std::next(elements_.begin(), std::distance(sorted_ids_.begin(), lower_bound));

	sorted_ids_.insert(lower_bound, id);
	return std::make_pair(elements_.insert(insert_element_at, element), true);
}

template<typename ElementType>
std::pair<typename IdMap<ElementType>::iterator, bool> IdMap<ElementType>::insert(
	const int     id,
	ElementType&& element)
{
	auto lower_bound = std::lower_bound(sorted_ids_.begin(), sorted_ids_.end(), id);

	if (lower_bound != sorted_ids_.end() && id == *lower_bound)
	{
		return std::make_pair(
			std::next(elements_.begin(), std::distance(sorted_ids_.begin(), lower_bound)), false);
	}

	auto insert_element_at =
		std::next(elements_.begin(), std::distance(sorted_ids_.begin(), lower_bound));

	sorted_ids_.insert(lower_bound, id);
	return std::make_pair(elements_.insert(insert_element_at, std::move(element)), true);
}

template<typename ElementType>
size_t IdMap<ElementType>::erase(const int id)
{
	auto lower_bound = std::lower_bound(sorted_ids_.begin(), sorted_ids_.end(), id);

	if (lower_bound == sorted_ids_.end() || id != *lower_bound)
	{
		return 0ull;
	}

	auto erase_element_at =
		std::next(elements_.begin(), std::distance(sorted_ids_.begin(), lower_bound));

	sorted_ids_.erase(lower_bound);
	elements_.erase(erase_element_at);

	return 1ull;
}

template<typename ElementType>
void IdMap<ElementType>::clear()
{
	elements_.clear();
	sorted_ids_.clear();
}

template<typename ElementType>
typename IdMap<ElementType>::iterator IdMap<ElementType>::find(const int id)
{
	const auto lower_bound = std::lower_bound(sorted_ids_.cbegin(), sorted_ids_.cend(), id);
	return (lower_bound == sorted_ids_.cend() || *lower_bound != id)
		? elements_.end()
		: std::next(elements_.begin(), std::distance(sorted_ids_.cbegin(), lower_bound));
}

template<typename ElementType>
typename IdMap<ElementType>::const_iterator IdMap<ElementType>::find(const int id) const
{
	const auto lower_bound = std::lower_bound(sorted_ids_.cbegin(), sorted_ids_.cend(), id);
	return (lower_bound == sorted_ids_.cend() || *lower_bound != id)
		? elements_.cend()
		: std::next(elements_.cbegin(), std::distance(sorted_ids_.cbegin(), lower_bound));
}

template<typename ElementType>
bool IdMap<ElementType>::contains(const int id) const
{
	const auto lower_bound = std::lower_bound(sorted_ids_.cbegin(), sorted_ids_.cend(), id);

	if (lower_bound == sorted_ids_.cend())
	{
		return false;
	}

	return *lower_bound == id;
}

// a very simple directional graph
// NOT USING GRAPH.H because the copy constructor makes it impossible to 
// do partial specialization
class NodeGraph
{
public:
	NodeGraph() 
		: current_id_(0)
		, nodes_()
		, edges_from_node_()
		, node_neighbors_()
		, edges_()
	    , m_rootId(-1)
	{}

	void setRoot(int id) { m_rootId = id; }

	void operator=(const NodeGraph& g) {
		edges_ = g.edges_;
		edges_from_node_ = g.edges_from_node_;
		node_neighbors_ = g.node_neighbors_;
		current_id_ = g.current_id_;
		nodes_ = g.nodes_;
	}

	virtual ~NodeGraph() {};

	struct Edge
	{
		int id;
		int from, to;

		Edge() = default;
		Edge(const int id, const int f, const int t) : id(id), from(f), to(t) {}

		inline int  opposite(const int n) const { return n == from ? to : from; }
		inline bool contains(const int n) const { return n == from || n == to; }
	};

	// Element access

	std::shared_ptr<Node>& node(int node_id);
	const std::shared_ptr<Node>& node(int node_id) const;
	Span<const int>  neighbors(int node_id) const;
	Span<const Edge> edges() const;

	// Capacity
	size_t num_edges_from_node(int node_id) const;

	// Modifiers
	int  insert_node(const std::shared_ptr<Node>& node);
	void erase_node(int node_id);

	int  insert_edge(int from, int to);
	void erase_edge(int edge_id);
	int current_id_;
	int getRoot() const { return m_rootId; }

private:

	// These contains map to the node id
	IdMap<std::shared_ptr<Node>>         nodes_;
	IdMap<int>              edges_from_node_;
	IdMap<std::vector<int>> node_neighbors_;

	// This container maps to the edge id
	IdMap<Edge> edges_;
	int m_rootId; 
	std::mutex m_mut;
};

template<typename Visitor>
void dfs_traverse(const NodeGraph& graph, Visitor visitor)
{
	int start_node = graph.getRoot();
	std::stack<int> stack;

	stack.push(start_node);

	while (!stack.empty())
	{
		const int current_node = stack.top();
		stack.pop();

		visitor(current_node);

		for (const int neighbor : graph.neighbors(current_node))
		{
			stack.push(neighbor);
		}
	}
}
