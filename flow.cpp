#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct flowEdge{
	int dest;
	T flow, capacity, cost;
	flowEdge *res;
	flowEdge(){
		this->dest = this->flow = this->capacity = this->cost = 0;
		this->res = NULL;
	}
	flowEdge(int dest, T flow, T capacity, T cost = 0){
		this->dest = dest, this->flow = flow, this->capacity = capacity, this->cost = cost;
		this->res = NULL;
	}
	void addFlow(T flow){
		this->flow += flow;
		this->res->flow -= flow;
	}
};

template<typename T>
struct flowGraph{
	T inf = numeric_limits<T>::max();
	vector<vector<flowEdge<T>*>> adjList;
	vector<int> dist, pos;
	int V;
	flowGraph(int V){
		this->V = V;
		adjList.resize(V);
		dist.resize(V);
		pos.resize(V);
	}
	~flowGraph(){
		for(int i = 0; i < V; ++i)
			for(int j = 0; j < adjList[i].size(); ++j)
				delete adjList[i][j];
	}
	void addEdge(int u, int v, T capacity, T cost = 0){
		flowEdge<T> *uv = new flowEdge<T>(v, 0, capacity, cost);
		flowEdge<T> *vu = new flowEdge<T>(u, capacity, capacity, -cost);
		uv->res = vu;
		vu->res = uv;
		adjList[u].push_back(uv);
		adjList[v].push_back(vu);
	}

	//Maximun Flow using Dinic Algorithm O(EV^2)
	T blockingFlow(int u, int t, T flow){
		if(u == t) return flow;
		for(int &i = pos[u]; i < adjList[u].size(); ++i){
			flowEdge<T> *v = adjList[u][i];
			if(v->capacity > v->flow && dist[u] + 1 == dist[v->dest]){
				T fv = blockingFlow(v->dest, t, min(flow, v->capacity - v->flow));
				if(fv > 0){
					v->addFlow(fv);
					return fv;
				}
			}
		}
		return 0;
	}
	T dinic(int s, int t){
		T maxFlow = 0;
		dist[t] = 0;
		while(dist[t] != -1){
			fill(dist.begin(), dist.end(), -1);
			queue<int> Q;
			Q.push(s);
			dist[s] = 0;
			while(!Q.empty()){
				int u = Q.front(); Q.pop();
				for(flowEdge<T> *v : adjList[u]){
					if(dist[v->dest] == -1 && v->flow != v->capacity){
						dist[v->dest] = dist[u] + 1;
						Q.push(v->dest);
					}
				}
			}
			if(dist[t] != -1){
				T f;
				fill(pos.begin(), pos.end(), 0);
				while(f = blockingFlow(s, t, inf))
					maxFlow += f;
			}
		}
		return maxFlow;
	}

	//Maximun Flow using Edmonds-Karp Algorithm O(VE^2)
	T edmondsKarp(int s, int t){
		T maxFlow = 0;
		vector<flowEdge<T>*> parent(V);
		while(true){
			fill(parent.begin(), parent.end(), nullptr);
			queue<int> Q;
			Q.push(s);
			while(!Q.empty() && !parent[t]){
				int u = Q.front(); Q.pop();
				for(flowEdge<T> *v : adjList[u]){
					if(!parent[v->dest] && v->capacity > v->flow){
						parent[v->dest] = v;
						Q.push(v->dest);
					}
				}
			}
			if(!parent[t]) break;
			T f = inf;
			for(int u = t; u != s; u = parent[u]->res->dest)
				f = min(f, parent[u]->capacity - parent[u]->flow);
			for(int u = t; u != s; u = parent[u]->res->dest)
				parent[u]->addFlow(f);
			maxFlow += f;
		}
		return maxFlow;
	}

	//Max Flow Min Cost
	pair<T, T> maxFlowMinCost(int s, int t){
		vector<bool> inQueue(V);
		vector<T> distance(V), cap(V);
		vector<flowEdge<T>*> parent(V);
		T maxFlow = 0, minCost = 0;
		while(true){
			fill(distance.begin(), distance.end(), inf);
			fill(parent.begin(), parent.end(), nullptr);
			fill(cap.begin(), cap.end(), 0);
			distance[s] = 0;
			cap[s] = inf;
			queue<int> Q;
			Q.push(s);
			while(!Q.empty()){
				int u = Q.front(); Q.pop(); inQueue[u] = 0;
				for(flowEdge<T> *v : adjList[u]){
					if(v->capacity > v->flow && distance[v->dest] > distance[u] + v->cost){
						distance[v->dest] = distance[u] + v->cost;
						parent[v->dest] = v;
						cap[v->dest] = min(cap[u], v->capacity - v->flow);
						if(!inQueue[v->dest]){
							Q.push(v->dest);
							inQueue[v->dest] = true;
						}
					}
				}
			}
			if(!parent[t]) break;
			maxFlow += cap[t];
			minCost += cap[t] * distance[t];
			for(int u = t; u != s; u = parent[u]->res->dest)
				parent[u]->addFlow(cap[t]);
		}
		return {maxFlow, minCost};
	}
};