#include <google/dense_hash_map>
#include <google/sparse_hash_map>
#include <iostream>
#include <string>

int main() {
    google::dense_hash_map<int, int> d_map;
    d_map.set_empty_key(-1);

    for (int i = 0; i < 100; ++i) {
        d_map[i] = i;
    }

    ++d_map[5];

    printf("%d\n", d_map[5]);

    google::sparse_hash_map<int, int> s_map;
    //const char *ptr = "ab";
    //const char *ptr2 = "ab";
    const int ptr = 2;
    const int ptr2 = 2;

    s_map[ptr] = 1;
    s_map[ptr2] = 2;

    printf("%d\n", s_map[ptr]);

    printf("%d\n", s_map.bucket(ptr2));
    printf("%.3f\n", s_map.load_factor());

    for (int i = 0; i < 10000000; ++i)
        s_map[i] = i;

    printf("///////////////\n%d\n", s_map[ptr]);

    printf("%d\n", s_map.bucket(ptr2));
    printf("%.3f\n", s_map.load_factor());
}
