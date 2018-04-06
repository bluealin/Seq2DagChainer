/*
 *
 * SequenceGraph.h
 *
 * Created on: Sept 17th 2017
 *	Author: aekuosma
 *
 */

#ifndef SEQUENCEGRAPH_H_
#define SEQUENCEGRAPH_H_

#include <vector>
#include <iostream>

#include "Vertex.h"
#include "Tuple.h"
#include "ColinearChain.h"
#include "MaximalExactMatch.h"

class SequenceGraph {

private:
	std::vector<Vertex*> vertices; // Vertices in topological order. 
				  // Currently the order comes from the file format (splicing graph files are ordered by genomic coordinates).

	// Mapping of splicing graph nodes to range of sequence graph nodes (required on converting colinear chains to subpaths)
	std::vector<std::pair<int,int> > splicingNodesToSequenceNodes;


	// Removes duplicates from the anchor list
	// TODO Should this be done while searching for anchors or is it more effective here?
	void removeDuplicates(std::vector<Tuple*> &anchors);

	void removePathOverlaps(std::vector<Tuple*> &anchors);


	/* The below functions are subroutines for naive anchor finding */

	// Fills the i'th entry of array D (the length of the longest suffix of i'th prefix that matches the pattern)
	// Also update the backpointers
	// Returns true if label of v matched the pattern, false otherwise
	// If i is outside pattern length, return false
	bool fillDArray(Vertex* v, int i, std::string pattern);

	void clearDArray(int patlen);

	// Reports matches that end at this vertex, for index i of D array
	// Report is in the form of ((start in pattern, end in pattern),(list of path nodes in graph))
		
	bool reportMatch(Vertex* v, int i, int threshold, std::vector<Tuple*> &results);

	// Helper recursive function for MEM->Tuple conversion
	void backtrackPath(std::vector<int> &path, std::string seq, int curpos, Vertex* curvertex);
	void backtrackReversePath(std::vector<int> &path, std::string seq, int curpos, Vertex* curvertex);


	void convertMEMToTuple(std::vector<Tuple*> &tuples, std::vector<Tuple*> &revtuples, MaximalExactMatch mem, std::string seq);

	// Helper function for convertChainToExons
	// If the colinear chain doesn't form a valid subpath, fix (e.g. no anchors on a short intervening exon)
	void fixPath(std::vector<int> &exons);

public:
	// Actual creation is done in the reading function
	SequenceGraph();

	void addToVertices(Vertex* v);

	// Destructor
	~SequenceGraph();
	
	int getNoOfVertices();

	// Returns i'th vertex
	Vertex* getVertex(int i);

	// Find and return all the anchors between a pattern and the sequence graph (old, naive method, slow)
	void findAnchorsNaive(std::vector<Tuple*> &results, std::string pattern, int threshold);

	// Find and return all the anchors using GSCA2
	void findAnchorsGCSA2(std::vector<Tuple*> &anchors, std::vector<Tuple*> &revanchors, std::string gcsa_name, std::string lcp_name, std::string pattern, int threshold);


	// Graph file has first row the number of nodes (exons), then the edges (i+1'th row has the outgoing edges from i'th node)
	// Nodes file has pairs of node number (first row) and the corresponding sequence (second row) for building the sequence graph (they need to be in order, labels are not checked currently!)
	// Patlen is the length of the reads (needed to initialize the arrays in vertex objects)
	bool readSplicingGraphFile(std::string graphfile, std::string nodesfile, int patlen);
	
	// Outputs the sequence graph as GFA
	// If concat is true, concatenate unary paths
	void outputGFA(std::string gfaout, bool concat);

	std::string convertChainToExons(ColinearChain chain);

};

#endif /* SEQUENCEGRAPH_H_ */
