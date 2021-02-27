#include <openssl/md5.h>
#include <string>

std::string md5(const std::string& path1, unsigned long size) {
    unsigned char c[MD5_DIGEST_LENGTH];
    int i;
    char *cs = new char[path1.length() + 1];
    strcpy(cs, path1.c_str());
    FILE *inFile = fopen (cs, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == nullptr) {
        printf ("%s can't be opened.\n", cs);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    fclose (inFile);
    return std::string ((reinterpret_cast<char*>(c)));
}