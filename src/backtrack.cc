/**
 * @file backtrack.cc
 *
 */

/*
#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";


  // implement your code here.
}

*/

/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

#include <queue>
#include <stack>
#include <array>

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query, const CandidateSet &cs) {
    std::cout << "t " << query.GetNumVertices() << "\n";
    // implement your code here.

    //root finding
    Vertex root = 0;
    double min = 1000000000;
    for(int u=0;u<(int)query.GetNumVertices();u++) {
        int c_ini_cnt = 0;
        //|c_ini(u)| calculation
        for(int v=0;v<(int)data.GetNumVertices();v++) {
            if((data.GetLabel(v) == query.GetLabel(u)) && data.GetDegree(v)>=query.GetDegree(u)) {
                c_ini_cnt++;
            }
        }
        //argmin calculation
        if(min > ((double)c_ini_cnt)/query.GetDegree(u)) {
            min = ((double)c_ini_cnt)/query.GetDegree(u);
            root = u;
        }
    }

    //build DAG with query using BFS
    // earlier to later visited vertices로 edge를 준다
    std::vector<std::vector<Vertex>> query_dag; //query_dag[i][j]는 i의 child가 j라는 의미 //요기?
    query_dag.resize(query.GetNumVertices());

    int dag_check[query.GetNumVertices()]; //아직 방문 안했으면 -1, 방문 순서대로 0 1 2 ... 번호가 배정
    for(int i=0;i<(int)query.GetNumVertices();i++) {
        dag_check[i] = -1;
    }


    //std::cout << "root :" << root << "\n";

    //DAG 만드는 용도의 queue
    std::queue<Vertex> q;
    int cnt = 0;
    q.push(root);
    dag_check[root] = cnt++;


    while(!q.empty()) {
        Vertex curr = q.front();
        q.pop();

        for(size_t i=query.GetNeighborStartOffset(curr) ; i<query.GetNeighborEndOffset(curr);i++) {
            Vertex next = query.GetNeighbor(i);
            if(dag_check[next] == -1) {
                query_dag[curr].push_back(next);
                q.push(next);
                dag_check[next] = cnt++;
            }
            else if(dag_check[next] > dag_check[curr]) {
                query_dag[curr].push_back(next);
            }
        }
    }

    /*
    std::cout << "query dag\n";
    for(int i=0;i<(int)query_dag.size();i++){
        std::cout << i << " : ";
        for(int j=0 ; j<(int)query_dag[i].size();j++) {
            std::cout << query_dag[i][j] << " ";
        }
        std::cout << "\n";
    }
     */

    //parent 탐색을 쉽게하기 위해 DAG의 inverse도 만들어 놓았다
    std::vector<std::vector<Vertex>> query_dag_inv; //query_dag_inv[i][j]는 i의 parent가 j라는 의미
    query_dag_inv.resize(query.GetNumVertices());
    for(int i=0;i<(int)query_dag.size();i++) {
        for(int j=0;j<(int)query_dag[i].size();j++) {
            query_dag_inv[query_dag[i][j]].push_back(i);
        }
    }

    /*
    std::cout << "query dag inv\n";
    for(int i=0;i<(int)query_dag_inv.size();i++){
        std::cout << i << " : ";
        for(int j=0 ; j<(int)query_dag_inv[i].size();j++) {
            std::cout << query_dag_inv[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "dag, inverse dag making done!\n" ;
     */

    //여기부터 본격적인 backtracking

    int embedding_size = query.GetNumVertices();
    Vertex embedding[embedding_size]; //여기다 embedding 나올때마다 저장
    for(int i=0;i<embedding_size;i++) {//embedding 초기화과정, -1이면 아직 matching이 안되었다는 의미
        embedding[i] = -1;
    }

    std::vector<Vertex> ext_vertex; //extendable vertex들의 벡터
    ext_vertex.push_back(root);


    std::vector<std::vector<std::pair<Vertex, Vertex>>> ext_candidate; //extendable candidate의 벡터 : pair의 앞이 u, 뒤가 v
    ext_candidate.resize(1);

    for(auto i=0 ; i<(int)cs.GetCandidateSize(root) ; i++) {
        auto curr_ext_candidate = cs.GetCandidate(root, i); // root에 match될 수 있는 cs들
        std::pair<Vertex, Vertex> a = std::make_pair(root, curr_ext_candidate);
        ext_candidate[0].push_back(a); // root에 match될 수 있다고 주어진 cs들은 무조건 extendable하다. root는 parent가 없기 때문
    }

    //여기까지 맨 처음 embedding (초기화된 상태), extendable vertex (현재 root) , extendable candidate 들을 구해놓았다

    //extendable candidate 중 하나를 골라서 extend
    for(int i=0;i<(int)ext_candidate[0].size();i++) {
        Vertex just_matched = ext_candidate[0][i].first;
        embedding[just_matched] = ext_candidate[0][i].second;
        //여기까지 확인 완료
        recur(embedding, just_matched, ext_vertex, ext_candidate, data, query, cs, query_dag, query_dag_inv);
    }
}


void Backtrack::recur(Vertex curr_embedding[], Vertex prev_matched, std::vector<Vertex> old_ext_vertex, std::vector<std::vector<std::pair<Vertex, Vertex>>> old_ext_candidate,
                      const Graph &data, const Graph &query, const CandidateSet &cs,
                      const std::vector<std::vector<Vertex>> &query_dag, const std::vector<std::vector<Vertex>> &query_dag_inv) {

    // 직전에 match된 vertex를 바탕으로 extendable vertex, candidate 다시 구하기
    std::pair<std::vector<Vertex>, std::vector<std::vector<std::pair<Vertex, Vertex>>>> new_pair =
            get_new_extendable_pair(old_ext_vertex, old_ext_candidate, prev_matched, curr_embedding, data, cs, query_dag, query_dag_inv);

    std::vector<Vertex> new_ext_vertex = new_pair.first;
    /*
    std::cout << "new_ext_vertex : ";
    for(int i=0;i<(int)new_ext_vertex.size();i++) {
        std::cout << new_ext_vertex[i] << " ";
    }
    std::cout << "\n";
     */

    std::vector<std::vector<std::pair<Vertex, Vertex>>> new_ext_candidate = new_pair.second;

    if(new_ext_vertex.empty()) {
        // 비어있다면 다 돌았거나 중간에 막혔거나 둘중 하나
        int size = query.GetNumVertices();
        for(int i=0;i<size;i++) {
            if(curr_embedding[i] == -1) { // 다 돌았다면 curr_embedding에 -1이 없어야함
                return;
            }
        }
        print_embedding(curr_embedding, size);
        return;
    }

    // new_ext_candidate는 크기 올림차순으로 sort 되어있는 상태
    // embedding을 업데이트 해주고 재귀
    for(int i=0;i<(int)new_ext_candidate[0].size();i++) {
        Vertex just_matched = new_ext_candidate[0][i].first;
        curr_embedding[just_matched] = new_ext_candidate[0][i].second; // extend 완료!
        recur(curr_embedding, just_matched, new_ext_vertex, new_ext_candidate, data, query, cs, query_dag, query_dag_inv);
        //recur 끝나서 돌아왔으면 넣어준 embedding 빼줘야 그 다음 후보를 넣어볼때 제대로 돌아감
        curr_embedding[just_matched] = -1;
    }

}

bool Backtrack::is_extendable_candidate(const std::pair<Vertex, Vertex> candidate, const Vertex curr_embedding[], const Graph &data, const std::vector<std::vector<Vertex>> &query_dag_inv) {
    Vertex u = candidate.first;
    Vertex v = candidate.second;
    for(int i=0;i<(int)query_dag_inv[u].size();i++) {
        Vertex parent_of_u = query_dag_inv[u][i]; //extendable vertex의 parent vertex들의 embedding들을 이후에 체크해야하기 때문

        if(curr_embedding[parent_of_u] == -1) {
            printf("Error in choosing extendable vertices"); // 여기로 왔다는 뜻은 extendable vertex u를 구할 떄 u의 모든 parent가 matching 된걸 골라야하는데 그렇지 못한것
            return false;
        }
        if(!data.IsNeighbor(curr_embedding[parent_of_u], v)) { //parent의 embedding과 extendable candidate of u가 연결이 되어있어야 한다
            return false;
        }

    }
    return true;
}

//new_extendable_vertex와 new_extendable_candidate의 pair를 구해서 리턴해줌
std::pair<std::vector<Vertex>, std::vector<std::vector<std::pair<Vertex, Vertex>>>> Backtrack::get_new_extendable_pair(std::vector<Vertex> old_extendable_vertex,
                                                                                                                       std::vector<std::vector<std::pair<Vertex, Vertex>>> old_extendable_candidate,
                                                                                                                       Vertex matched, const Vertex curr_embedding[], const Graph &data, const CandidateSet &cs,
                                                                                                                       const std::vector<std::vector<Vertex>> &query_dag,
                                                                                                                       const std::vector<std::vector<Vertex>> &query_dag_inv){

    /* 주요 논리
     * 1. 새로운 extendable vertex들은 기존 extendable vertex에서 방금 match된 vertex를 빼고
     * 그 vertex의  child 중 (child의 모든 parent)가 embedding에서 match되어 있는 child만 골라서
     * 새로운 extendable vertex들에 추가해준다.
     *
     * 2. 새로운 extendable candidate들은 기존 extendable candidate 중 방금 match된 vertex가 포함된 pair 전체를 빼고
     * (즉 한 줄 전체를 제외시켜야함, 같은 후보군에 있는 vertex들은 추가로 matching이 불가능하기 때문)
     * 새로 추가된 extendable vertex들을 탐색하면서 각각의 extendable candidate을 구한 후 추가
    */

    /*
    std::cout << "old_ext_vertex before: "; //수정전
    for(int i=0;i<(int)old_extendable_vertex.size();i++) {
        std::cout << old_extendable_vertex[i] << " ";
    }
    std::cout << "\n";
     */

    for(int i=0;i<(int)old_extendable_vertex.size();i++) {
        if(old_extendable_vertex[i] == matched) {
            old_extendable_vertex.erase(old_extendable_vertex.begin()+i);
            break;
        }
    }

    /*
    std::cout << "old_ext_vertex after 1: "; // 1차 수정 (매칭된거 제외)
    for(int i=0;i<(int)old_extendable_vertex.size();i++) {
        std::cout << old_extendable_vertex[i] << " ";
    }
    std::cout << "\n";
     */

    for(int i=0 ; i<(int)old_extendable_candidate.size();i++) {
        if(old_extendable_candidate[i][0].first == matched) {
            old_extendable_candidate.erase(old_extendable_candidate.begin()+i);
            break;
        }
    }



    std::vector<Vertex> new_extendable_vertex; //이걸 old에다 append 할것

    for(Vertex child : query_dag[matched]) { //방금 match한거의 child들을 검토
        bool checker = true;
        for(Vertex parent_of_child : query_dag_inv[child]) {
            if(curr_embedding[parent_of_child] == -1) { //parent가 모두 matching되지는 않는 child
                checker = false;
                break;
            }
        }
        if(checker) {
            new_extendable_vertex.push_back(child);
            std::vector<std::pair<Vertex, Vertex>> new_extendable_candidate; //이걸 old의 적당한 위치(size sort)에 끼워넣을것

            for(int i=0;i<(int)cs.GetCandidateSize(child);i++) {
                Vertex v = cs.GetCandidate(child, i); //candidate of child
                std::pair<Vertex, Vertex> a = std::make_pair(child, v);
                if(is_extendable_candidate(a, curr_embedding, data, query_dag_inv)) {
                    new_extendable_candidate.push_back(a);
                }
            }

            bool inserted = false;
            for(int pos=0;pos<(int)old_extendable_candidate.size();pos++) {
                if(new_extendable_candidate.size() <= old_extendable_candidate[pos].size()) { // size 정렬을 유지하도록 끼워넣어줌
                    old_extendable_candidate.insert(old_extendable_candidate.begin()+pos, new_extendable_candidate);
                    inserted = true;
                    break;
                }
            }

            //새로 만들어진 candidate 크기가 가장 클 때는 그냥 append
            if(!inserted) {
                old_extendable_candidate.insert(old_extendable_candidate.end(), new_extendable_candidate);
            }
        }
    }

    /*
    std::cout << "old_ext_candidate : ";
    std::cout << "\n";
    for(int i=0;i<(int)old_extendable_candidate.size();i++) {
        for(int j=0;j<(int)old_extendable_candidate[i].size();j++) {
            std::cout << "(" << old_extendable_candidate[i][j].first << " " << old_extendable_candidate[i][j].second << ")";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    */

    /*
    std::cout << "new_ext_vertex : ";
    for(int i=0;i<(int)new_extendable_vertex.size();i++) {
        std::cout << new_extendable_vertex[i] << " ";
    }
    std::cout << "\n";

    */


    old_extendable_vertex.insert(old_extendable_vertex.end(), new_extendable_vertex.begin(), new_extendable_vertex.end());

    return std::make_pair(old_extendable_vertex, old_extendable_candidate);

}

void Backtrack::print_embedding(const Vertex curr_embedding[], int size) {

    std::cout << "a";
    for(int i=0;i<size;i++) {
        std::cout  << " " << curr_embedding[i];
    }
    std::cout << "\n";
}