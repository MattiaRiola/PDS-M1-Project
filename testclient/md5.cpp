#include <openssl/md5.h>
#include <openssl/sha.h>
#include <string>

std::string md5(const std::string &path1) {
    unsigned char c[MD5_DIGEST_LENGTH];
    char *cs = new char[path1.length() + 1];
    strcpy(cs, path1.c_str());
    FILE *inFile = fopen (cs, "rb");
    MD5_CTX mdContext;
    //SHA_CTX sc;
    int bytes;
    unsigned char data[1024];

    if (inFile == nullptr) {
        printf ("%s can't be opened.\n", cs);
        return nullptr;
    }



    MD5_Init (&mdContext);
    //SHA1_Init(&sc);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        // SHA1_Update(&sc, data, bytes);
        MD5_Update (&mdContext, data, bytes);

    MD5_Final (c,&mdContext);
    //SHA1_Final(c, &sc);
    fclose (inFile);
    return std::string ((reinterpret_cast<char*>(c)));
}