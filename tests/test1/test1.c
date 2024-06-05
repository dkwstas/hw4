#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    char text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Hendrerit dolor magna eget est lorem ipsum dolor. Felis bibendum ut tristique et egestas. Turpis cursus in hac habitasse. Nullam non nisi est sit amet facilisis magna. Malesuada fames ac turpis egestas sed. Egestas sed tempus urna et pharetra. Bibendum arcu vitae elementum curabitur vitae nunc sed velit. Elit sed vulputate mi sit amet mauris commodo quis imperdiet. Et magnis dis parturient montes nascetur ridiculus mus mauris vitae. Tempor orci eu lobortis elementum nibh. Dui nunc mattis enim ut. Hac habitasse platea dictumst vestibulum rhoncus est. Justo laoreet sit amet cursus sit amet dictum sit. Praesent tristique magna sit amet purus gravida quis. Adipiscing elit pellentesque habitant morbi tristique senectus et netus et. Tristique magna sit amet purus. Vitae suscipit tellus mauris a diam maecenas sed enim ut. Venenatis a condimentum vitae sapien pellentesque habitant morbi. Dolor magna eget est lorem ipsum dolor sit amet consectetur. Mauris vitae ultricies leo integer malesuada nunc. Fermentum dui faucibus in ornare quam. Adipiscing bibendum est ultricies integer. Consectetur purus ut faucibus pulvinar elementum integer enim neque volutpat. Enim lobortis scelerisque fermentum dui faucibus in ornare quam. Non pulvinar neque laoreet suspendisse. Orci sagittis eu volutpat odio facilisis. Felis donec et odio pellentesque diam volutpat commodo sed egestas. Iaculis nunc sed augue lacus viverra vitae. Massa sapien faucibus et molestie ac feugiat sed. Tincidunt eget nullam non nisi est. In hac habitasse platea dictumst quisque. Ullamcorper sit amet risus nullam eget felis. Pretium fusce id velit ut tortor pretium. Tempor commodo ullamcorper a lacus vestibulum. Eleifend quam adipiscing vitae proin sagittis. Accumsan in nisl nisi scelerisque eu ultrices. Felis imperdiet proin fermentum leo vel. Purus sit amet volutpat consequat mauris nunc. Sit amet volutpat consequat mauris. Nunc pulvinar sapien et ligula ullamcorper malesuada proin libero nunc. Aliquet eget sit amet tellus. Mauris ultrices eros in cursus turpis. Sed ullamcorper morbi tincidunt ornare. Aliquam purus sit amet luctus venenatis. Enim sed faucibus turpis in eu mi. Pharetra diam sit amet nisl. Pellentesque sit amet porttitor eget. Orci dapibus ultrices in iaculis nunc sed augue lacus viverra. Sed tempus urna et pharetra pharetra massa massa. Neque viverra justo nec ultrices dui. Odio eu feugiat pretium nibh ipsum consequat nisl vel pretium. Viverra accumsan in nisl nisi scelerisque eu ultrices. Tortor condimentum lacinia quis vel eros donec ac odio. Vitae semper quis lectus nulla at. Elit duis tristique sollicitudin nibh sit amet commodo nulla facilisi. Sed risus pretium quam vulputate dignissim suspendisse in est. Sed adipiscing diam donec adipiscing tristique risus nec. Vitae congue mauris rhoncus aenean vel elit scelerisque mauris pellentesque. Dolor morbi non arcu risus quis. Tempor nec feugiat nisl pretium fusce. In nisl nisi scelerisque eu. Mi proin sed libero enim sed faucibus turpis in. Vitae semper quis lectus nulla at volutpat diam ut. Nibh nisl condimentum id venenatis a condimentum vitae sapien. Ultrices vitae auctor eu augue ut lectus. Egestas maecenas pharetra convallis posuere morbi leo. Cursus vitae congue mauris rhoncus aenean vel. Arcu odio ut sem nulla. Ipsum faucibus vitae aliquet nec. Adipiscing elit pellentesque habitant morbi tristique senectus. Purus viverra accumsan in nisl nisi scelerisque eu. Odio facilisis mauris sit amet massa vitae tortor condimentum.";

    if (argc != 2) {
        printf("Invalid args.\n");
        return(-1);
    }

    char *token = strtok(text, "i");
    while (token != NULL) {
        write(STDOUT_FILENO, token, strlen(token));
        usleep(100000);
        token = strtok(NULL, "i");
    }

    return(0);
}