class Graph:
	def __init__(self):
		self.Nodes ={}
		self.Edges = []

	def __addNode(self, a, b):
		if not a in self.Nodes:
			self.Nodes[a] = set([])
		self.Nodes[a].add(b)

	def addEdge(self, fr, to):
		self.Edges += [(fr,to)]
		self.__addNode(fr,to)
		if not to in self.Nodes:
			self.Nodes[to] = set([])

	def removeedge(self, fr, to):
		if (fr, to) in self.Edges:
			self.Edges.remove((fr, to))
		self.Edges[fr].remove(to)
		
	def __str__(self):
		return str(self.Nodes)

	def __visit(self, node, visited):
		visited.add(node)
		for i in [j for j in self.Nodes[node] if not j in visited]:
			self.__visit(i, visited)


	def isConnected(self):
		visited = set([])
		self.__nodes = list(self.Nodes.keys())
		self.__visit(self.__nodes[0], visited)
		if visited == set(self.Nodes.keys()):
			return True
		return False

