/************************************************************************/
/* $Id: MainP.cpp 65 2010-09-08 06:48:36Z yan.qi.asu $                                                                 */
/************************************************************************/

/* This code has been modified to conform to the ICGCA format by the   *
 * ICGCA organizers in May 2023.  See https://afsa.jp/icgca/ on ICGCA. */

#include <limits>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include "GraphElements.h"
#include "Graph.h"
#include "DijkstraShortestPathAlg.h"
#include "YenTopKShortestPathsAlg.h"

using namespace std;


void testDijkstraGraph()
{
	Graph* my_graph_pt = new Graph("test_15");
	DijkstraShortestPathAlg shortest_path_alg(my_graph_pt);
	BasePath* result =
		shortest_path_alg.get_shortest_path(
			my_graph_pt->get_vertex(0), my_graph_pt->get_vertex(14));
	result->PrintOut(cout);
}

int testYenAlg(char *p, int s, int t, int k)
{
	Graph my_graph(p);

	YenTopKShortestPathsAlg yenAlg(my_graph, my_graph.get_vertex(s),
			my_graph.get_vertex(t));

	int i=0;
	while(yenAlg.has_next())
	{
		BasePath* cur_p = yenAlg.next();
		double w = cur_p->Weight();
		if (w > k) break;
		++i;
	}

	return i;
}

// int main(...)
int main(int argc, char *argv[])
{
	char tmpfile[L_tmpnam];
	tmpnam(tmpfile);
	FILE *fp = fopen(tmpfile, "wt");

	char buf[64];
	int n, m, k;
	int s = -1, t = -1;
	while (fgets(buf, sizeof(buf), stdin))
	{
		if (buf[0] == 'p')
		{
			sscanf(buf, "p edge %d %d", &n, &m);
			fprintf(fp, "%d\n\n", n);
		}
		else if (buf[0] == 'e') {
			int u, v;
			sscanf(buf, "e %d %d", &u, &v);
			fprintf(fp, "%d %d 1\n", u, v);
			fprintf(fp, "%d %d 1\n", v, u);
		}
		else if (buf[0] == 'l') {
			sscanf(buf, "l %d", &k);
		}
		else if (buf[0] == 't') {
			sscanf(buf, "t %d %d", &s, &t);
		}
	}
	fclose(fp);

	if (s > 0) {
		printf("%d", testYenAlg(tmpfile, s, t, k));
	}
	else {
		int n_paths = 0;
		for (int s = 1; s <= n; ++s)
		{
			for (int t = s + 1; t <= n; ++t)
			{
				n_paths += testYenAlg(tmpfile, s, t, k);
			}
		}
		printf("%d", n_paths);
	}

	remove(tmpfile);

	return 0;
}
