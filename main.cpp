#include <cassert>
#include "HHBench.h"
#include "HCBench.h"
#include "HHHBench.h"
#include "HHH2Bench.h"

std::string folder = "../datasets/";

std::string file[1] = {"130000.dat"};

int main(int argc,char* argv[]) {
    if (argc != 2)
    {
        std::cout << "Parameters Error!" << std::endl;
        return 0;
    }

    int exp_type = atoi(argv[1]);
    if(exp_type == 1){
        std::cout << "Heavy Hitters:" << std::endl;
        HHBench HH(folder + file[0], "CAIDA");
        HH.HHSingleBench(500000, 0.0001);
        HH.HHMultiBench(500000, 0.0001);
    }else if(exp_type == 2){
        std::cout << "Heavy Changes:" << std::endl;
        HCBench HC(folder + file[0], "CAIDA");
        HC.HCSingleBench(500000, 0.0001);
        HC.HCMultiBench(500000, 0.0001);
    }else if(exp_type == 3){
        std::cout << "1-D Hierarchical Heavy Hitters:" << std::endl;
        HHHBench HHH(folder + file[0], "CAIDA");
        for(uint32_t i = 1;i <= 5;++i){
            std::cout << "Memory:" << i * 500000 / 1000 << "KB" << std::endl;
            HHH.HHHSingleBench(i * 500000, 0.00005);
            HHH.HHHMultiBench(i * 500000, 0.00005);
        }
    }else if(exp_type == 4){
        std::cout << "2-D Hierarchical Heavy Hitters:" << std::endl;
        HHH2Bench HHH2(folder + file[0], "CAIDA");
        for(uint32_t i = 1;i <= 5;++i){
            std::cout << "Memory:" << i * 5000000 / 1000000 << "MB" << std::endl;
            HHH2.HHH2SingleBench(i * 5000000, 0.00005);
            HHH2.HHH2MultiBench(i * 5000000, 0.00005);
        } 
    }

    return 0;
}