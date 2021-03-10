#include <deque>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>

#define Edge std::pair<int, int>

class Graph {	// class for graphs object
private:
	std::vector<Edge> *graph;	// this vector keeps edges of the graphs
	int vertex_no;				// the number of vertices
public:
	Graph(const int &vertex_no) noexcept;											// constructor for graph objects
	~Graph() noexcept;																// destructor for graph objects
	int get_no_of_vertices() const noexcept;										// returns number of vertices of graph object
	std::vector<Edge>*& get_graph() noexcept;										// returns the whole graph object with edges
	std::vector<Edge> get_vertex_edges(const int &v) const noexcept;				// returns the edges of the specific vertex
	int get_no_of_vertex_edges(const int &v) const noexcept;						// number of edges of a vertex
	Edge get_edge_from_vertex(const int &v, const int &e) const noexcept;			// returns a specific edge from a specific vertex
	void add_edge(const int &from, const int &to, const int &w) noexcept;			// adds an edge to a specific vertex
	void change_edges(const int &v, const std::vector<Edge> &changed) noexcept;		// replaces edges of the specific vertex
};
Graph::Graph(const int &vertex_no) noexcept {											// constructor of the graph object
	this->vertex_no = vertex_no;							
	this->graph = new std::vector<Edge>[this->vertex_no];	
}
Graph::~Graph() noexcept {																// destructor of the graph
	
	delete [] graph;
}
int Graph::get_no_of_vertices() const noexcept {										// this is a basic method to take number of vertices
	
	return vertex_no;	
}
std::vector<Edge>*& Graph::get_graph() noexcept {										// this is a basic method to take edge vector of graphs
	
	return graph;
}
std::vector<Edge> Graph::get_vertex_edges(const int &v) const noexcept {				// returns the edges of the specific vertex
	
	return graph[v];
}
int Graph::get_no_of_vertex_edges(const int &v) const noexcept {						// returns number of edges of a vertex
	
	return graph[v].size();
}
Edge Graph::get_edge_from_vertex(const int &v, const int &e) const noexcept {			// returns a specific edge from a specific vertex
	
	return graph[v][e];
}
void Graph::add_edge(const int &from, const int &to, const int &w) noexcept {			// adds an edge to a specific vertex 
	graph[from].emplace_back(to, w);
}
void Graph::change_edges(const int &v, const std::vector<Edge> &changed) noexcept {		// replaces edges of he specific vertex
	graph[v] = changed;		
}


class MinCostMaxFlow {	// min cost max flow class
private:
	Graph graph;															// graph to make min cost max flow
	int detect_cycle(const int &v, int *pre) const noexcept;				// method for detecting negative cycle
	int check_cycle(int *pre) const noexcept;								// detects negative cycles in the graph
	std::list<int> get_cycle(int v, int *pre) const noexcept;				// returns negative cycle as list 
public:
	MinCostMaxFlow(const int &vertex_no) noexcept;							// constructor of the object
	void add_edge(const int &from, const int &to, const int &w) noexcept;	// adds an edge from one vertex to another
	void reduce_cycles() noexcept;											// deletes this negative cycle from graph by making opeartions on it
	int get_cost() const noexcept;											// returns total cost of flow
};

MinCostMaxFlow::MinCostMaxFlow(const int &vertex_no) noexcept : graph(vertex_no) { }	

void MinCostMaxFlow::add_edge(const int &from, const int &to, const int &w) noexcept {
	graph.add_edge(from, to, w);
}

int MinCostMaxFlow::detect_cycle(const int &v, int *pre) const noexcept {	// this traverse graph and detects if there are a negative cycle or not
	
	static std::list<int> vec;
	const int &size = graph.get_no_of_vertices();	
	bool visited[size], on_stack[size];

	for(int g=size-1; g>=0; g--){

		visited[g]=false;
		on_stack[g]=false;
	}

	for(int i = size-1; i >= 0; --i)
		if(!visited[i]) {
			for(int j = pre[v]; ~j; j = pre[j])
				if(visited[j]) {
					if(on_stack[j])
						return j;					// if there are a negative cycle returns a member of it
					break;
				}
				else {
					vec.push_back(j);
					visited[j] = on_stack[j] = true;
				}
			for(const int &j : vec)
				on_stack[j] = false;
			vec.clear();
		}
	return -1;										// if there are not a negative cycle returns -1
}

int MinCostMaxFlow::check_cycle(int *pre) const noexcept {				// checks if there are a negative cycle or not
	const int &size = graph.get_no_of_vertices();
	int v_to_check, dist[size];
	bool inQueue[size];
	std::deque<int> Deque;
	for(int i = size-1; i >= 0; Deque.push_back(i--)) {
		pre[i] = -1;
		inQueue[i] = true;
		dist[i]=0;
	}
	for(int iter = 0; Deque.size();) {
		const int &v = Deque.back(), &edge_no = graph.get_no_of_vertex_edges(v);
		inQueue[v] = false;
		Deque.pop_back();
		for(int e = 0; e < edge_no; ++e) {
			const Edge &edge = graph.get_edge_from_vertex(v, e);
			const int &new_dist = dist[v] + edge.second, &t = edge.first;
			if(new_dist < dist[t]) {
				v_to_check = t;
				dist[v_to_check] = new_dist;
				pre[v_to_check] = v;
				if((++iter) == size) {
					const int &check_cycle = detect_cycle(v_to_check, pre);
					if(~check_cycle)
						return check_cycle;
					iter = 0;
				}
				if(!inQueue[t]) {
					inQueue[t] = !inQueue[t];
					Deque.push_front(t);
				}
			}
		}
	}
	return detect_cycle(v_to_check, pre);
}

void MinCostMaxFlow::reduce_cycles() noexcept {						// deletes negative cycle from the graph
	int pre[graph.get_no_of_vertices()];
	for(int v_cycle = check_cycle(pre); ~v_cycle; v_cycle = check_cycle(pre)) {
		const std::list<int> &cycle = get_cycle(v_cycle, pre);
		for(auto it = cycle.crbegin(); (++it) != cycle.crend();) {
			--it;
			const int &v1 = *it, &size_v1 = graph.get_no_of_vertex_edges(v1);
			++it;
			const int &v2 = *it;
			static int w;
			static std::vector<Edge> changed;
			for(int j = 0; j < size_v1; ++j) {
				const Edge &edge = graph.get_edge_from_vertex(v1, j);
				if(edge.first != v2)
					changed.push_back(edge);
				else
					w = edge.second;
			}
			graph.add_edge(v2, v1, -w);
			graph.change_edges(v1, changed);
			changed.clear();
		}
	}
}

std::list<int> MinCostMaxFlow::get_cycle(int v, int *pre) const noexcept {		// returns the negative cylce as a list
	std::list<int> cycle;
	cycle.push_front(v);
	do {
		cycle.push_front(v);
		v = pre[v];
		for(auto it = cycle.crbegin(); it != cycle.crend(); ++it)
			if(*it == v) {
				const int &init = cycle.back();
				cycle.pop_back();
				cycle.push_front(init);
				cycle.reverse();
				return cycle;
			}
	} while(true);
}

int MinCostMaxFlow::get_cost() const noexcept {								
	int total_cost = 0;
	for(int i = graph.get_no_of_vertices() - 1 ; i >= 0; --i)
		for(int j = graph.get_no_of_vertex_edges(i) - 1 ; j >= 0; --j) {
			const int &cost = graph.get_edge_from_vertex(i, j).second;
			if(cost > 0)
				total_cost += cost;
		}
	return total_cost;
}

int main(int argc, char* argv[]) {


	int T;
	std::ifstream is(argv[1]);
	is >> T;
	std::ofstream os(argv[2]);
	while(T--) {
		static int N;
		is >> N;
		MinCostMaxFlow mcmf(2*N);
		for(int r = 0; r < N; ++r)
			for(int c = 0; c < N; ++c) {
				static int w;
				is >> w;
				if(r == c)
					mcmf.add_edge(c+N, r, w);
				else
					mcmf.add_edge(r, c+N, -w);
			}
		mcmf.reduce_cycles();
		os << mcmf.get_cost() << std::endl;
	}
	is.close();
	os.close();


	return 0;
}
