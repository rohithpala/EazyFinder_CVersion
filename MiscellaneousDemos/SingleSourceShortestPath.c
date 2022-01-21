#include<stdio.h>
#include<limits.h>

void SingleSourceShortestPath(int n, int adj_mat[][n], int sourceVertex){
    int i, j, k, shortestPath[n], visited[n], min_dist, min_dist_vertex;

    // Initializations
    for(i = 0 ; i < n ; i++){
        if(adj_mat[sourceVertex][i])
            shortestPath[i] = adj_mat[sourceVertex][i];
        else
            shortestPath[i] = INT_MAX;
        visited[i] = 0;
    }
    shortestPath[sourceVertex] = 0;

    // Main Code
    for(i = 0 ; i < n ; i++){
        min_dist = INT_MAX;
        for(k = 0 ; k < n ; k++){
            // Select the vertices that are unvisited and are nearer to sourceVertex
            if(visited[k] == 0 && min_dist >= shortestPath[k]){
                min_dist = shortestPath[k];
                min_dist_vertex = k;
            }
        }
        // min_dist_vertex is at minimum distance to sourceVertex. So put it in solution
        visited[min_dist_vertex] = 1;

        for (j = 0; j < n; j++){
            // Select unvisited vertices, adjacent to min_dist_vertex and
            // distance from source to this vertex is not infinity
            // Also apply the relaxation procedure
            if(!visited[j] && adj_mat[min_dist_vertex][j] && shortestPath[min_dist_vertex] != INT_MAX && 
                shortestPath[j] > shortestPath[min_dist_vertex] + adj_mat[min_dist_vertex][j]){
                    shortestPath[j] = shortestPath[min_dist_vertex] + adj_mat[min_dist_vertex][j];
            }
        }
          
    }

    // Printing the shortest paths
    printf("Shortest Paths from the Source Vertex %d:\n", sourceVertex);
    for(i = 0; i < n; i++){
        printf("%d -> %d: %d\n", sourceVertex, i, shortestPath[i]);
    }
}

void main()
{
    int n, i, j, sourceVertex;
    printf("Enter the Number of Nodes: ");
    scanf("%d", &n);
    int adj_mat[n][n];
    //Taking Edges Matrix as Input
    printf("Enter the Edges Matrix:\n");
    for(i = 0 ; i < n ; i++){
        for(j = 0 ; j < n ; j++){
            scanf("%d", &adj_mat[i][j]);
        }
    }

    printf("\n");

    printf("Enter the Source Vertex: ");
    scanf("%d", &sourceVertex);
    SingleSourceShortestPath(n, adj_mat, sourceVertex);
}
