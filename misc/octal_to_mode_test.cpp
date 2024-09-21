#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sys/stat.h>

#define SUID 04000
#define SGID 02000
#define SVTX 01000
#define RUSR 00400
#define WUSR 00200
#define XUSR 00100
#define RGRP 00040
#define WGRP 00020
#define XGRP 00010
#define ROTH 00004
#define WOTH 00002
#define XOTH 00001
#define ALLM 07777 /* all octal mode bits */

static mode_t octal_to_mode(unsigned int octal) {
    /* Help the compiler optimize the usual case where mode_t uses
       the traditional octal representation.  */
    return ((S_ISUID == SUID && S_ISGID == SGID && S_ISVTX == SVTX &&
             S_IRUSR == RUSR && S_IWUSR == WUSR && S_IXUSR == XUSR &&
             S_IRGRP == RGRP && S_IWGRP == WGRP && S_IXGRP == XGRP &&
             S_IROTH == ROTH && S_IWOTH == WOTH && S_IXOTH == XOTH)
                ? octal
                : (mode_t)((octal & SUID ? S_ISUID : 0) |
                           (octal & SGID ? S_ISGID : 0) |
                           (octal & SVTX ? S_ISVTX : 0) |
                           (octal & RUSR ? S_IRUSR : 0) |
                           (octal & WUSR ? S_IWUSR : 0) |
                           (octal & XUSR ? S_IXUSR : 0) |
                           (octal & RGRP ? S_IRGRP : 0) |
                           (octal & WGRP ? S_IWGRP : 0) |
                           (octal & XGRP ? S_IXGRP : 0) |
                           (octal & ROTH ? S_IROTH : 0) |
                           (octal & WOTH ? S_IWOTH : 0) |
                           (octal & XOTH ? S_IXOTH : 0)));
}
static mode_t str_to_mode(const char *mode) {
    const char *p = mode;
    unsigned int octal_mode = 0;

    do {
        octal_mode = 8 * octal_mode + *p++ - '0';
        if (ALLM < octal_mode)
            std::cerr << "invalid octal_mode: " << octal_mode << '\n';
    } while ('0' <= *p && *p < '8');

    return octal_to_mode(octal_mode);
}

TEST_CASE("octal_to_mode", "case1") {
    REQUIRE(str_to_mode("755") == 0b111101101);
    REQUIRE(str_to_mode("0755") == 0b111101101);
    REQUIRE(str_to_mode("644") == 0b110100100);
    REQUIRE(str_to_mode("0644") == 0b110100100);
}