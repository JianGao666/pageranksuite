#include "pagerank.h"
#include <string.h>
#include <unistd.h>

int INPUT_SIZE = sizeof(bench_args_t);

void run_benchmark( void *vargs ) {
  bench_args_t *args = (bench_args_t *)vargs;
  pagerank(args->nodes, args->edges, args->pagerank_cur, args->pagerank_next);
}

/* Input format:
%% Section 1
uint64_t[1]: starting node
%% Section 2
uint64_t[N_NODES*2]: node structures (start and end indices of edge lists)
%% Section 3
uint64_t[N_EDGES]: edges structures (just destination node id)
*/

void input_to_data(int fd, void *vdata) {
  bench_args_t *data = (bench_args_t *)vdata;
  char *p, *s;
  uint64_t *nodes;
  int64_t i;
  // Zero-out everything.
  memset(vdata,0,sizeof(bench_args_t));
  // // Max-ify levels
  // for(i=0; i<N_NODES; i++) {
  //   data->level[i]=MAX_LEVEL;
  // }
  // Load input string
  p = readfile(fd);
  // // Section 1: starting node
  // s = find_section_start(p,1);
  // parse_uint64_t_array(s, &data->starting_node, 1);

  // Section 2: node structures
  s = find_section_start(p,1);
  nodes = (uint64_t *)malloc(N_NODES*2*sizeof(uint64_t));
  parse_uint64_t_array(s, nodes, N_NODES*2);
  for(i=0; i<N_NODES; i++) {
    data->nodes[i].edge_begin = nodes[2*i];
    data->nodes[i].edge_end = nodes[2*i+1];
  }
  free(nodes);
  // Section 3: edge structures
  s = find_section_start(p,2);
  parse_uint64_t_array(s, (uint64_t *)(data->edges), N_EDGES);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  uint64_t *nodes;
  int64_t i;

  bench_args_t *data = (bench_args_t *)vdata;
  // // Section 1: starting node
  // write_section_header(fd);
  // write_uint64_t_array(fd, &data->starting_node, 1);
  // Section 2: node structures
  write_section_header(fd);
  nodes = (uint64_t *)malloc(N_NODES*2*sizeof(uint64_t));
  for(i=0; i<N_NODES; i++) {
    nodes[2*i]  = data->nodes[i].edge_begin;
    nodes[2*i+1]= data->nodes[i].edge_end;
  }
  write_uint64_t_array(fd, nodes, N_NODES*2);
  free(nodes);
  // Section 3: edge structures
  write_section_header(fd);
  write_uint64_t_array(fd, (uint64_t *)(&data->edges), N_EDGES);
}

/* Output format:
%% Section 1
uint64_t[N_LEVELS]: horizon counts
*/

void output_to_data(int fd, void *vdata) {
  bench_args_t *data = (bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(bench_args_t));
  // Load input string
  p = readfile(fd);
  // Section 1: horizon counts
  s = find_section_start(p,1);
  parse_double_array(s, data->pagerank_cur, N_NODES);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  bench_args_t *data = (bench_args_t *)vdata;
  // Section 1
  write_section_header(fd);
  write_double_array(fd, data->pagerank_cur, N_NODES);
}

double fabs(double a);

int check_data( void *vdata, void *vref ) {
  bench_args_t *data = (bench_args_t *)vdata;
  bench_args_t *ref = (bench_args_t *)vref;
  int has_errors = 0;
  int i;

  // Check that the horizons have the same number of nodes
  for(i=0; i<N_NODES; i++) {
    has_errors |= fabs(data->pagerank_cur[i]-ref->pagerank_cur[i]) > 0.001;
  }

  // Return true if it's correct.
  return !has_errors;
}

// initialization for the pagerank_cur and pagerank_next
void init( void *vargs ) {
  bench_args_t *args = (bench_args_t *)vargs;
  pagerank_init(args->pagerank_cur, args->pagerank_next);
}

void pagerank_init(pagerank_t pagerank_cur[N_NODES], pagerank_t pagerank_next[N_NODES])
{
  for (int i = 0; i < N_NODES; i++){
        pagerank_cur[i] = 1.0 / N_NODES;
        pagerank_next[i] = 0;
    }
}