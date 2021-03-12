#include <utility>


#ifndef TCP_FCB_H
#define TCP_FCB_H
class fcb{

public:

    std::mutex m;
    std::condition_variable cv;
    std::string name;
    std::string md5;
    bool ok;
    int bloccoatteso;

    fcb(const fcb& f1) {
        name = f1.name;
        ok = f1.ok;
        md5 = f1.md5;
        bloccoatteso = f1.bloccoatteso;
    };

    fcb( std::string name, std::string md5, bool stato){
        ok = stato;
        this->name = std::move(name);
        this->md5 = std::move(md5);
        this->bloccoatteso = 0;

    };


    void updatebloccoatteso(){
        this->bloccoatteso ++;
        if(bloccoatteso == 1000)
            bloccoatteso = 0;
    };
};
#endif //TCP_FCB_H
