## Pagerank_suite



Pagerank_suite [1] is a HLS benchmark suite designed specifically for the page rank algorithm [2] on the unweighted directed graph. There are two versions of the pagerank algorithm. The first one is called as pagerank naive which traverse each node to calculate the pagerank value. The second one is called as pagerank edgemap which traverse each edge to calculate the pagerank value. 

### Run

1 . type `make generate` to generate `input.data` and `input.data2`.

2 . run `python ./check.py` to generate `check.data`.

2 . type `make run` to run the code.

### HLS

1 . run `make hls`.

### Synthesize Verilog code

1 . run `make syn`.

### Formula

1 .  ![pagerank_formula](https://sitebulb.com/media/1818/pagerank-formula.png)

2 . Above image is the formula used to calculate the pagerank value. We first calculate the last part which is the sum of the source node divided by its out degree. Then, we time its value with a damping factor d and add a constant which is the first part of the formula to it. 

### Q&A

##### Input

1 . What is `input.data`?

`input.data` represents a synthesized graph data generated by `generate.c`. 

2 . What does the data inside the `input.data` mean?

Here is a example of `input.data`:

![Screenshot 2021-08-26 112407](https://user-images.githubusercontent.com/84207576/130999798-788108a1-eeb2-4b3a-91fb-fe14f9b3069b.png)

The first half of the data under `%%` represents the node structure. The second half of the data under `%%` represent the edge structure. Each two index in the node structure represent the start edge index and end edge index point out from the node. And the interval of the edge index is left close and right open. For example, the node 0 inside the node structure has two edge index `0` and `1` which means node 0 only has one edge point out from it. Since `0` in the edge structure represent `2` , there is an edge point from node 0 to node 2. As a result, the above example `input.data` represent a graph as following:

![Screenshot 2021-08-26 111943](https://user-images.githubusercontent.com/84207576/130999361-450d9d64-6e48-412a-a5cb-8f6b6f8a816c.png)

3 . How to adjust the `input.data`?

Since all the `input.data` represent synthesized graph, we can adjust graph by using mainly two ways. First, we can use `pagerank.h` to control the graph density. There are following lines of the code inside the `pagerank.h`:

```c
// Terminology (but not values) from graph500 spec
//   graph density = 2^-(2*SCALE - EDGE_FACTOR)
#define SCALE 2
#define EDGE_FACTOR 1

#define N_NODES (1<<SCALE)
#define N_EDGES (N_NODES*EDGE_FACTOR)
```

![Screenshot 2021-10-01 231031](https://user-images.githubusercontent.com/84207576/135703223-d01a903c-7c64-4191-ae73-8d33afd4c32e.png)

Their relationship can use above formula to represent. In the above case, there are 4 nodes and 4 edges. Since the basic topology we used to generate the synthesized graph is RMAT algorithm [3], we can use the following code inside the `generate.c` to control the graph we synthesized.

```c
// R-MAT parameters (*100)
#if 1
// Scale-free, small-world graph
#define A 57
#define B 19
#define C 19
#define D 5
#else
```

The basic theory behind the RMAT algorithm is that we recursively subdivide the adjacency matrix into four equal-sized partitions , and distribute edges within these partitions with an unequal probabilities. Each edge chooses one of the four partitions with probabilities A, B, C, and D. The chosen partition again will be divided into 4 equal-sized smaller partitions, and the procedure will repeat until we reach a simple 1x1 partition. Then, we will assign an edge to the 1x1 partition. The above code help we control the possibility to assign an edge to its corresponding partition. 

4 . What is `input.data2`?

`input.data2` is the matrix version of the `input.data` used for the input of `check.py`.

5 . What does the data inside `input.data2` mean?

The first line represent the number of nodes. The rest of line represent the adjacency matrix of the input.

##### Check

1 . What is `check.py`?

`check.py` use the matrix version of the input as the `pagerank.c` to check if the pagerank value generated by the `pagerank.c` is correct. We used matrix arithmetic to calculate the pagerank value for `check.py`. We compare the `check.data` and `output.data` to determine if our result correct. We consider the result is correct if the difference is less than 0.001.

2 . What is `check.data`?

`check.data` represents the pagerank value for each node.

##### Output

1 . What is output?

Both `edgemap` and `naive` print the output directly to the terminal which represents the pagerank value for each node.

##### Others

1 . What is the difference between `edgemap` and `naive`?

`naive` traverse all the nodes while `edgemap` traverse all the edges.

### Reference

> [1] S. Brin and L. Page, “The anatomy of a large-scale hypertextual web search engine,” *Computer Networks and ISDN Systems*, vol. 30, no. 1-7, pp. 107–117, 1998. 
>
> [2] Brandon Reagen, Robert Adolf, Sophia Yakun Shao, Gu-Yeon Wei, and David Brooks.
> *"MachSuite: Benchmarks for Accelerator Design and Customized Architectures."*
> 2014 IEEE International Symposium on Workload Characterization.
>
> [3] D. Chakrabarti, Y. Zhan, and C. Faloutsos, “R-mat: A recursive model for graph mining,” *Proceedings of the 2004 SIAM International Conference on Data Mining*, 2004. 

