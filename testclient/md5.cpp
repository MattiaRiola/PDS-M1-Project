#include <openssl/md5.h>
#include <string>
#include <sstream>



std::string md5(const std::string &path1) {
    unsigned char c[MD5_DIGEST_LENGTH];
    char *cs = new char[path1.length() + 1];
    strcpy(cs, path1.c_str());
    FILE *inFile = fopen (cs, "rb");
    MD5_CTX mdContext;

    int bytes;
    unsigned char data[1024];

    if (inFile == nullptr) {
        printf ("%s can't be opened.\n", cs);
        return nullptr;
    }



    MD5_Init (&mdContext);

    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);

    MD5_Final (c,&mdContext);
    fclose (inFile);

//    //OLD_version: Problem: if the char has 0 it wasn't printed  in the result string
//    std::stringstream hex_result;
//    for(int i = 0; i<MD5_DIGEST_LENGTH; ++i)
//        hex_result << std::hex << (int)c[i];
//    std::string hex_result1 = hex_result.str();

    std::string hex_result2;
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for( int i = 0; i < MD5_DIGEST_LENGTH; ++i )
    {
        unsigned char const byte = c[i];
        hex_result2 += hex_chars[ ( byte & 0xF0 ) >> 4 ];
        hex_result2 += hex_chars[ ( byte & 0x0F ) >> 0 ];
    }

    return hex_result2;
}
