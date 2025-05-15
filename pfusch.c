#include <stdio.h>

typedef struct Edge {
    int from;
    int to;
    int weight;
} Edge;

int main(void) {
    int verticeamount = 4;
    Edge edges[] = {
        {0, 1, 1}, {0, 2, 1}, {0, 3, 1},
        {1, 0, 1}, {1, 2, 1}, {1, 3, 1},
        {2, 0, 1}, {2, 1, 1}, {2, 3, 1},
        {3, 0, 1}, {3, 1, 1}, {3, 2, 1}
    };
    int edgecount = sizeof(edges) / sizeof(edges[0]);

    // Header
    printf("+  ");
    for (int i = 0; i < verticeamount; i++) {
        printf("%d  ", i);
    }
    printf("\n");

    // Separator
    printf("  +");
    for (int i = 0; i <= verticeamount; i++) {
        printf("--");
    }
    printf("\n");

    // Matrix
    for (int j = 0; j < verticeamount; j++) {
        printf("%d |", j);
        for (int k = 0; k < verticeamount; k++) {
            if (j == k) {
                printf("0  ");
            } else {
                for (int e = 0; e < edgecount; e++) {
                    if (edges[e].from == j && edges[e].to == k) {
                        printf("%d  ", edges[e].weight);
                        break;
                    }
                }
            }
        }
        printf("\n");
    }

    return 0;
}
