#include <stdio.h>
#include <string.h>

void read_omf_file(const char *path) {
    FILE *pf = fopen(path, "rb");
    if (pf == NULL) {
        printf("error\n");
        return;
    }

    unsigned long long cprs_feat = 0;
    int real[16];
    const int mask =  (1 << 4) - 1;
    printf("%d %d\n", 0xF - 1,  mask);
    while (1) {
        int res = 0;
        if ((res = fread(&cprs_feat, sizeof(unsigned char), 8, pf)) != 8) {
            printf("%d\n",res);
            break;
        }
        //printf("%llu\n", feat);
        for (int i = 0; i < 16; i++) {
            real[15 - i] = cprs_feat & mask;    
            cprs_feat >>= 4;
        }    

//        for (int i = 0; i < 16; ++i) {
//            printf("%d ", real[i]);
//        }
//        printf("\n");
    }

    fclose(pf);
}

void read_file() {
    FILE *pf = fopen("filename.list", "r");
    if (pf == NULL) {
        return;
    }

    char buf[128];
    while (fgets(buf, 128, pf) != NULL) {
        //printf("%s", buf);
        buf[strlen(buf) - 1] = '\0';
        read_omf_file(buf);
    }
}

int main() {
    read_file();
}
