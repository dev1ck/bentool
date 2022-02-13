#include "wifi.h"
struct test
{
    uint32_t a;
    char b;
    char c[];
}__attribute__((__packed__));

int main()
{
    struct test *test;
    char * str = "abcdefg";
    test = (struct test*)str;
    printf("%d",sizeof(*test));

    return 0;

}