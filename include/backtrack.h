/**
 * @file backtrack.h
 *
 */
/*
#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Backtrack {
 public:
  Backtrack();
  ~Backtrack();

  void PrintAllMatches(const Graph &data, const Graph &query,
                       const CandidateSet &cs);
};

#endif  // BACKTRACK_H_
*/

/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include <stack>
#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Backtrack {
public:
    Backtrack();
    ~Backtrack();

    void PrintAllMatches(const Graph &data, const Graph &query, const CandidateSet &cs);

    void recur(Vertex curr_embedding[], Vertex prev_matched, std::vector<Vertex> old_ext_vertex, std::vector<std::vector<std::pair<Vertex, Vertex>>> old_ext_candidate,
               const Graph &data, const Graph &query, const CandidateSet &cs,
               const std::vector<std::vector<Vertex>> &query_dag, const std::vector<std::vector<Vertex>> &query_dag_inv);

    bool is_extendable_candidate(std::pair<Vertex, Vertex> candidate, const Vertex curr_embedding[], const Graph &data, const std::vector<std::vector<Vertex>> &query_dag_inv);

    std::pair<std::vector<Vertex>, std::vector<std::vector<std::pair<Vertex, Vertex>>>> get_new_extendable_pair(std::vector<Vertex> old_extendable_vertex,
                                                                                                                std::vector<std::vector<std::pair<Vertex, Vertex>>> old_extendable_candidate,
                                                                                                                Vertex matched, const Vertex curr_embedding[], const Graph &data, const CandidateSet &cs,
                                                                                                                const std::vector<std::vector<Vertex>> &query_dag,
                                                                                                                const std::vector<std::vector<Vertex>> &query_dag_inv);

    void print_embedding(const Vertex curr_embedding[], int size);

};

#endif  // BACKTRACK_H_
