#ifndef HHH2BENCH_H
#define HHH2BENCH_H

#include <vector>
#include <fstream>

#include "Univmon.h"
#include "Elastic.h"
#include "CMHeap.h"
#include "CountHeap.h"
#include "SpaceSaving.h"

#include "OurSoft.h"
#include "USS.h"

#include "MMap.h"

#define HHH2OtherSketch SpaceSaving
#define LAYER2 1089

class HHH2Bench{
public:

    HHH2Bench(std::string PATH, std::string name){
        dataset = read_data(PATH.c_str(), 100000000, &length);

        for(uint64_t i = 0;i < length;++i)
            mp[dataset[i].srcIP_dstIP()] += 1;
    }

    ~HHH2Bench(){
        delete dataset;
    }

    void HHH2SingleBench(uint32_t MEMORY, double alpha){
        SingleAbstract<uint64_t>* sketch[LAYER2];
        uint32_t mem = MEMORY / LAYER2;

        for(uint32_t i = 0;i < LAYER2;++i)
            sketch[i] = new HHH2OtherSketch<uint64_t>(mem);

        for(uint32_t i = 0;i < length;++i){
            uint32_t pos = randomGenerator() % LAYER2;
            sketch[pos]->Insert(dataset[i].srcIP_dstIP() & MASK[pos]);
        }

        COUNT_TYPE threshold = alpha * length;

        double realHH = 0, estHH = 0, bothHH = 0, aae = 0, are = 0;

        for(uint32_t i = 0;i < LAYER2;++i){
            std::unordered_map<uint64_t, COUNT_TYPE> tempMp, tempEstMp;

            tempEstMp = sketch[i]->AllQuery();
            for(auto it = tempEstMp.begin(); it != tempEstMp.end();++it){
                tempEstMp[it->first] = tempEstMp[it->first] * LAYER2;
            }

            for(auto it = mp.begin(); it != mp.end();++it)
                tempMp[it->first & MASK[i]] += it->second;

            for(auto it = tempMp.begin();it != tempMp.end();++it){
                bool real, est;
                double realF = it->second, estF = tempEstMp[it->first];

                real = (realF > threshold);
                est = (estF > threshold);

                realHH += real;
                estHH += est;

                if(real && est){
                    bothHH += 1;
                    aae += abs(realF - estF);
                    are += abs(realF - estF) / realF;
                }
            }
        }

        std::cout << "recall," << bothHH / realHH << std::endl;
        std::cout << "precision," << bothHH / estHH  << std::endl;
        std::cout << "aae," << aae / bothHH << std::endl;
        std::cout << "are," << are / bothHH << std::endl;
        std::cout << std::endl;

        for(uint32_t i = 0;i < LAYER2;++i)
            delete sketch[i];
    }

    void HHH2MultiBench(uint32_t MEMORY, double alpha){
        MultiAbstract<uint64_t>* sketch = new OurSoft<uint64_t>(MEMORY);

        for(uint32_t i = 0;i < length;++i){
            sketch->Insert(dataset[i].srcIP_dstIP());
        }

        std::unordered_map<uint64_t, COUNT_TYPE> estMp = sketch->AllQuery();

        COUNT_TYPE threshold = alpha * length;

        double realHH = 0, estHH = 0, bothHH = 0, aae = 0, are = 0;

        for(uint32_t i = 0;i < LAYER2;++i){
            std::unordered_map<uint64_t, COUNT_TYPE> tempMp, tempEstMp;

            for(auto it = estMp.begin(); it != estMp.end();++it)
                tempEstMp[it->first & MASK[i]] += it->second;

            for(auto it = mp.begin(); it != mp.end();++it)
                tempMp[it->first & MASK[i]] += it->second;

            for(auto it = tempMp.begin();it != tempMp.end();++it){
                bool real, est;
                double realF = it->second, estF = tempEstMp[it->first];

                real = (realF > threshold);
                est = (estF > threshold);

                realHH += real;
                estHH += est;

                if(real && est){
                    bothHH += 1;
                    aae += abs(realF - estF);
                    are += abs(realF - estF) / realF;
                }
            }
        }

        std::cout << "recall," << bothHH / realHH << std::endl;
        std::cout << "precision," << bothHH / estHH  << std::endl;
        std::cout << "aae," << aae / bothHH << std::endl;
        std::cout << "are," << are / bothHH << std::endl;
        std::cout << std::endl;

        delete sketch;
    }

private:

    TUPLES* dataset;
    uint64_t length;

    std::unordered_map<uint64_t, COUNT_TYPE> mp;

    const uint64_t MASK[LAYER2] = {0xffffffffffffffffL, 0xfffffffffffffffeL, 0xfffffffeffffffffL, 0xfffffffffffffffcL, 0xfffffffefffffffeL, 0xfffffffcffffffffL, 0xfffffffffffffff8L, 0xfffffffefffffffcL, 0xfffffffcfffffffeL,
                                   0xfffffff8ffffffffL, 0xfffffffffffffff0L, 0xfffffffefffffff8L, 0xfffffffcfffffffcL, 0xfffffff8fffffffeL, 0xfffffff0ffffffffL, 0xffffffffffffffe0L, 0xfffffffefffffff0L, 0xfffffffcfffffff8L, 0xfffffff8fffffffcL,
                                   0xfffffff0fffffffeL, 0xffffffe0ffffffffL, 0xffffffffffffffc0L, 0xfffffffeffffffe0L, 0xfffffffcfffffff0L, 0xfffffff8fffffff8L, 0xfffffff0fffffffcL, 0xffffffe0fffffffeL, 0xffffffc0ffffffffL, 0xffffffffffffff80L,
                                   0xfffffffeffffffc0L, 0xfffffffcffffffe0L, 0xfffffff8fffffff0L, 0xfffffff0fffffff8L, 0xffffffe0fffffffcL, 0xffffffc0fffffffeL, 0xffffff80ffffffffL, 0xffffffffffffff00L, 0xfffffffeffffff80L, 0xfffffffcffffffc0L,
                                   0xfffffff8ffffffe0L, 0xfffffff0fffffff0L, 0xffffffe0fffffff8L, 0xffffffc0fffffffcL, 0xffffff80fffffffeL, 0xffffff00ffffffffL, 0xfffffffffffffe00L, 0xfffffffeffffff00L, 0xfffffffcffffff80L, 0xfffffff8ffffffc0L,
                                   0xfffffff0ffffffe0L, 0xffffffe0fffffff0L, 0xffffffc0fffffff8L, 0xffffff80fffffffcL, 0xffffff00fffffffeL, 0xfffffe00ffffffffL, 0xfffffffffffffc00L, 0xfffffffefffffe00L, 0xfffffffcffffff00L, 0xfffffff8ffffff80L,
                                   0xfffffff0ffffffc0L, 0xffffffe0ffffffe0L, 0xffffffc0fffffff0L, 0xffffff80fffffff8L, 0xffffff00fffffffcL, 0xfffffe00fffffffeL, 0xfffffc00ffffffffL, 0xfffffffffffff800L, 0xfffffffefffffc00L, 0xfffffffcfffffe00L,
                                   0xfffffff8ffffff00L, 0xfffffff0ffffff80L, 0xffffffe0ffffffc0L, 0xffffffc0ffffffe0L, 0xffffff80fffffff0L, 0xffffff00fffffff8L, 0xfffffe00fffffffcL, 0xfffffc00fffffffeL, 0xfffff800ffffffffL, 0xfffffffffffff000L,
                                   0xfffffffefffff800L, 0xfffffffcfffffc00L, 0xfffffff8fffffe00L, 0xfffffff0ffffff00L, 0xffffffe0ffffff80L, 0xffffffc0ffffffc0L, 0xffffff80ffffffe0L, 0xffffff00fffffff0L, 0xfffffe00fffffff8L, 0xfffffc00fffffffcL,
                                   0xfffff800fffffffeL, 0xfffff000ffffffffL, 0xffffffffffffe000L, 0xfffffffefffff000L, 0xfffffffcfffff800L, 0xfffffff8fffffc00L, 0xfffffff0fffffe00L, 0xffffffe0ffffff00L, 0xffffffc0ffffff80L, 0xffffff80ffffffc0L,
                                   0xffffff00ffffffe0L, 0xfffffe00fffffff0L, 0xfffffc00fffffff8L, 0xfffff800fffffffcL, 0xfffff000fffffffeL, 0xffffe000ffffffffL, 0xffffffffffffc000L, 0xfffffffeffffe000L, 0xfffffffcfffff000L, 0xfffffff8fffff800L,
                                   0xfffffff0fffffc00L, 0xffffffe0fffffe00L, 0xffffffc0ffffff00L, 0xffffff80ffffff80L, 0xffffff00ffffffc0L, 0xfffffe00ffffffe0L, 0xfffffc00fffffff0L, 0xfffff800fffffff8L, 0xfffff000fffffffcL, 0xffffe000fffffffeL,
                                   0xffffc000ffffffffL, 0xffffffffffff8000L, 0xfffffffeffffc000L, 0xfffffffcffffe000L, 0xfffffff8fffff000L, 0xfffffff0fffff800L, 0xffffffe0fffffc00L, 0xffffffc0fffffe00L, 0xffffff80ffffff00L, 0xffffff00ffffff80L,
                                   0xfffffe00ffffffc0L, 0xfffffc00ffffffe0L, 0xfffff800fffffff0L, 0xfffff000fffffff8L, 0xffffe000fffffffcL, 0xffffc000fffffffeL, 0xffff8000ffffffffL, 0xffffffffffff0000L, 0xfffffffeffff8000L, 0xfffffffcffffc000L,
                                   0xfffffff8ffffe000L, 0xfffffff0fffff000L, 0xffffffe0fffff800L, 0xffffffc0fffffc00L, 0xffffff80fffffe00L, 0xffffff00ffffff00L, 0xfffffe00ffffff80L, 0xfffffc00ffffffc0L, 0xfffff800ffffffe0L, 0xfffff000fffffff0L,
                                   0xffffe000fffffff8L, 0xffffc000fffffffcL, 0xffff8000fffffffeL, 0xffff0000ffffffffL, 0xfffffffffffe0000L, 0xfffffffeffff0000L, 0xfffffffcffff8000L, 0xfffffff8ffffc000L, 0xfffffff0ffffe000L, 0xffffffe0fffff000L,
                                   0xffffffc0fffff800L, 0xffffff80fffffc00L, 0xffffff00fffffe00L, 0xfffffe00ffffff00L, 0xfffffc00ffffff80L, 0xfffff800ffffffc0L, 0xfffff000ffffffe0L, 0xffffe000fffffff0L, 0xffffc000fffffff8L, 0xffff8000fffffffcL,
                                   0xffff0000fffffffeL, 0xfffe0000ffffffffL, 0xfffffffffffc0000L, 0xfffffffefffe0000L, 0xfffffffcffff0000L, 0xfffffff8ffff8000L, 0xfffffff0ffffc000L, 0xffffffe0ffffe000L, 0xffffffc0fffff000L, 0xffffff80fffff800L,
                                   0xffffff00fffffc00L, 0xfffffe00fffffe00L, 0xfffffc00ffffff00L, 0xfffff800ffffff80L, 0xfffff000ffffffc0L, 0xffffe000ffffffe0L, 0xffffc000fffffff0L, 0xffff8000fffffff8L, 0xffff0000fffffffcL, 0xfffe0000fffffffeL,
                                   0xfffc0000ffffffffL, 0xfffffffffff80000L, 0xfffffffefffc0000L, 0xfffffffcfffe0000L, 0xfffffff8ffff0000L, 0xfffffff0ffff8000L, 0xffffffe0ffffc000L, 0xffffffc0ffffe000L, 0xffffff80fffff000L, 0xffffff00fffff800L,
                                   0xfffffe00fffffc00L, 0xfffffc00fffffe00L, 0xfffff800ffffff00L, 0xfffff000ffffff80L, 0xffffe000ffffffc0L, 0xffffc000ffffffe0L, 0xffff8000fffffff0L, 0xffff0000fffffff8L, 0xfffe0000fffffffcL, 0xfffc0000fffffffeL,
                                   0xfff80000ffffffffL, 0xfffffffffff00000L, 0xfffffffefff80000L, 0xfffffffcfffc0000L, 0xfffffff8fffe0000L, 0xfffffff0ffff0000L, 0xffffffe0ffff8000L, 0xffffffc0ffffc000L, 0xffffff80ffffe000L, 0xffffff00fffff000L,
                                   0xfffffe00fffff800L, 0xfffffc00fffffc00L, 0xfffff800fffffe00L, 0xfffff000ffffff00L, 0xffffe000ffffff80L, 0xffffc000ffffffc0L, 0xffff8000ffffffe0L, 0xffff0000fffffff0L, 0xfffe0000fffffff8L, 0xfffc0000fffffffcL,
                                   0xfff80000fffffffeL, 0xfff00000ffffffffL, 0xffffffffffe00000L, 0xfffffffefff00000L, 0xfffffffcfff80000L, 0xfffffff8fffc0000L, 0xfffffff0fffe0000L, 0xffffffe0ffff0000L, 0xffffffc0ffff8000L, 0xffffff80ffffc000L,
                                   0xffffff00ffffe000L, 0xfffffe00fffff000L, 0xfffffc00fffff800L, 0xfffff800fffffc00L, 0xfffff000fffffe00L, 0xffffe000ffffff00L, 0xffffc000ffffff80L, 0xffff8000ffffffc0L, 0xffff0000ffffffe0L, 0xfffe0000fffffff0L,
                                   0xfffc0000fffffff8L, 0xfff80000fffffffcL, 0xfff00000fffffffeL, 0xffe00000ffffffffL, 0xffffffffffc00000L, 0xfffffffeffe00000L, 0xfffffffcfff00000L, 0xfffffff8fff80000L, 0xfffffff0fffc0000L, 0xffffffe0fffe0000L,
                                   0xffffffc0ffff0000L, 0xffffff80ffff8000L, 0xffffff00ffffc000L, 0xfffffe00ffffe000L, 0xfffffc00fffff000L, 0xfffff800fffff800L, 0xfffff000fffffc00L, 0xffffe000fffffe00L, 0xffffc000ffffff00L, 0xffff8000ffffff80L,
                                   0xffff0000ffffffc0L, 0xfffe0000ffffffe0L, 0xfffc0000fffffff0L, 0xfff80000fffffff8L, 0xfff00000fffffffcL, 0xffe00000fffffffeL, 0xffc00000ffffffffL, 0xffffffffff800000L, 0xfffffffeffc00000L, 0xfffffffcffe00000L,
                                   0xfffffff8fff00000L, 0xfffffff0fff80000L, 0xffffffe0fffc0000L, 0xffffffc0fffe0000L, 0xffffff80ffff0000L, 0xffffff00ffff8000L, 0xfffffe00ffffc000L, 0xfffffc00ffffe000L, 0xfffff800fffff000L, 0xfffff000fffff800L,
                                   0xffffe000fffffc00L, 0xffffc000fffffe00L, 0xffff8000ffffff00L, 0xffff0000ffffff80L, 0xfffe0000ffffffc0L, 0xfffc0000ffffffe0L, 0xfff80000fffffff0L, 0xfff00000fffffff8L, 0xffe00000fffffffcL, 0xffc00000fffffffeL,
                                   0xff800000ffffffffL, 0xffffffffff000000L, 0xfffffffeff800000L, 0xfffffffcffc00000L, 0xfffffff8ffe00000L, 0xfffffff0fff00000L, 0xffffffe0fff80000L, 0xffffffc0fffc0000L, 0xffffff80fffe0000L, 0xffffff00ffff0000L,
                                   0xfffffe00ffff8000L, 0xfffffc00ffffc000L, 0xfffff800ffffe000L, 0xfffff000fffff000L, 0xffffe000fffff800L, 0xffffc000fffffc00L, 0xffff8000fffffe00L, 0xffff0000ffffff00L, 0xfffe0000ffffff80L, 0xfffc0000ffffffc0L,
                                   0xfff80000ffffffe0L, 0xfff00000fffffff0L, 0xffe00000fffffff8L, 0xffc00000fffffffcL, 0xff800000fffffffeL, 0xff000000ffffffffL, 0xfffffffffe000000L, 0xfffffffeff000000L, 0xfffffffcff800000L, 0xfffffff8ffc00000L,
                                   0xfffffff0ffe00000L, 0xffffffe0fff00000L, 0xffffffc0fff80000L, 0xffffff80fffc0000L, 0xffffff00fffe0000L, 0xfffffe00ffff0000L, 0xfffffc00ffff8000L, 0xfffff800ffffc000L, 0xfffff000ffffe000L, 0xffffe000fffff000L,
                                   0xffffc000fffff800L, 0xffff8000fffffc00L, 0xffff0000fffffe00L, 0xfffe0000ffffff00L, 0xfffc0000ffffff80L, 0xfff80000ffffffc0L, 0xfff00000ffffffe0L, 0xffe00000fffffff0L, 0xffc00000fffffff8L, 0xff800000fffffffcL,
                                   0xff000000fffffffeL, 0xfe000000ffffffffL, 0xfffffffffc000000L, 0xfffffffefe000000L, 0xfffffffcff000000L, 0xfffffff8ff800000L, 0xfffffff0ffc00000L, 0xffffffe0ffe00000L, 0xffffffc0fff00000L, 0xffffff80fff80000L,
                                   0xffffff00fffc0000L, 0xfffffe00fffe0000L, 0xfffffc00ffff0000L, 0xfffff800ffff8000L, 0xfffff000ffffc000L, 0xffffe000ffffe000L, 0xffffc000fffff000L, 0xffff8000fffff800L, 0xffff0000fffffc00L, 0xfffe0000fffffe00L,
                                   0xfffc0000ffffff00L, 0xfff80000ffffff80L, 0xfff00000ffffffc0L, 0xffe00000ffffffe0L, 0xffc00000fffffff0L, 0xff800000fffffff8L, 0xff000000fffffffcL, 0xfe000000fffffffeL, 0xfc000000ffffffffL, 0xfffffffff8000000L,
                                   0xfffffffefc000000L, 0xfffffffcfe000000L, 0xfffffff8ff000000L, 0xfffffff0ff800000L, 0xffffffe0ffc00000L, 0xffffffc0ffe00000L, 0xffffff80fff00000L, 0xffffff00fff80000L, 0xfffffe00fffc0000L, 0xfffffc00fffe0000L,
                                   0xfffff800ffff0000L, 0xfffff000ffff8000L, 0xffffe000ffffc000L, 0xffffc000ffffe000L, 0xffff8000fffff000L, 0xffff0000fffff800L, 0xfffe0000fffffc00L, 0xfffc0000fffffe00L, 0xfff80000ffffff00L, 0xfff00000ffffff80L,
                                   0xffe00000ffffffc0L, 0xffc00000ffffffe0L, 0xff800000fffffff0L, 0xff000000fffffff8L, 0xfe000000fffffffcL, 0xfc000000fffffffeL, 0xf8000000ffffffffL, 0xfffffffff0000000L, 0xfffffffef8000000L, 0xfffffffcfc000000L,
                                   0xfffffff8fe000000L, 0xfffffff0ff000000L, 0xffffffe0ff800000L, 0xffffffc0ffc00000L, 0xffffff80ffe00000L, 0xffffff00fff00000L, 0xfffffe00fff80000L, 0xfffffc00fffc0000L, 0xfffff800fffe0000L, 0xfffff000ffff0000L,
                                   0xffffe000ffff8000L, 0xffffc000ffffc000L, 0xffff8000ffffe000L, 0xffff0000fffff000L, 0xfffe0000fffff800L, 0xfffc0000fffffc00L, 0xfff80000fffffe00L, 0xfff00000ffffff00L, 0xffe00000ffffff80L, 0xffc00000ffffffc0L,
                                   0xff800000ffffffe0L, 0xff000000fffffff0L, 0xfe000000fffffff8L, 0xfc000000fffffffcL, 0xf8000000fffffffeL, 0xf0000000ffffffffL, 0xffffffffe0000000L, 0xfffffffef0000000L, 0xfffffffcf8000000L, 0xfffffff8fc000000L,
                                   0xfffffff0fe000000L, 0xffffffe0ff000000L, 0xffffffc0ff800000L, 0xffffff80ffc00000L, 0xffffff00ffe00000L, 0xfffffe00fff00000L, 0xfffffc00fff80000L, 0xfffff800fffc0000L, 0xfffff000fffe0000L, 0xffffe000ffff0000L,
                                   0xffffc000ffff8000L, 0xffff8000ffffc000L, 0xffff0000ffffe000L, 0xfffe0000fffff000L, 0xfffc0000fffff800L, 0xfff80000fffffc00L, 0xfff00000fffffe00L, 0xffe00000ffffff00L, 0xffc00000ffffff80L, 0xff800000ffffffc0L,
                                   0xff000000ffffffe0L, 0xfe000000fffffff0L, 0xfc000000fffffff8L, 0xf8000000fffffffcL, 0xf0000000fffffffeL, 0xe0000000ffffffffL, 0xffffffffc0000000L, 0xfffffffee0000000L, 0xfffffffcf0000000L, 0xfffffff8f8000000L,
                                   0xfffffff0fc000000L, 0xffffffe0fe000000L, 0xffffffc0ff000000L, 0xffffff80ff800000L, 0xffffff00ffc00000L, 0xfffffe00ffe00000L, 0xfffffc00fff00000L, 0xfffff800fff80000L, 0xfffff000fffc0000L, 0xffffe000fffe0000L,
                                   0xffffc000ffff0000L, 0xffff8000ffff8000L, 0xffff0000ffffc000L, 0xfffe0000ffffe000L, 0xfffc0000fffff000L, 0xfff80000fffff800L, 0xfff00000fffffc00L, 0xffe00000fffffe00L, 0xffc00000ffffff00L, 0xff800000ffffff80L,
                                   0xff000000ffffffc0L, 0xfe000000ffffffe0L, 0xfc000000fffffff0L, 0xf8000000fffffff8L, 0xf0000000fffffffcL, 0xe0000000fffffffeL, 0xc0000000ffffffffL, 0xffffffff80000000L, 0xfffffffec0000000L, 0xfffffffce0000000L,
                                   0xfffffff8f0000000L, 0xfffffff0f8000000L, 0xffffffe0fc000000L, 0xffffffc0fe000000L, 0xffffff80ff000000L, 0xffffff00ff800000L, 0xfffffe00ffc00000L, 0xfffffc00ffe00000L, 0xfffff800fff00000L, 0xfffff000fff80000L,
                                   0xffffe000fffc0000L, 0xffffc000fffe0000L, 0xffff8000ffff0000L, 0xffff0000ffff8000L, 0xfffe0000ffffc000L, 0xfffc0000ffffe000L, 0xfff80000fffff000L, 0xfff00000fffff800L, 0xffe00000fffffc00L, 0xffc00000fffffe00L,
                                   0xff800000ffffff00L, 0xff000000ffffff80L, 0xfe000000ffffffc0L, 0xfc000000ffffffe0L, 0xf8000000fffffff0L, 0xf0000000fffffff8L, 0xe0000000fffffffcL, 0xc0000000fffffffeL, 0x80000000ffffffffL, 0xffffffff00000000L,
                                   0xfffffffe80000000L, 0xfffffffcc0000000L, 0xfffffff8e0000000L, 0xfffffff0f0000000L, 0xffffffe0f8000000L, 0xffffffc0fc000000L, 0xffffff80fe000000L, 0xffffff00ff000000L, 0xfffffe00ff800000L, 0xfffffc00ffc00000L,
                                   0xfffff800ffe00000L, 0xfffff000fff00000L, 0xffffe000fff80000L, 0xffffc000fffc0000L, 0xffff8000fffe0000L, 0xffff0000ffff0000L, 0xfffe0000ffff8000L, 0xfffc0000ffffc000L, 0xfff80000ffffe000L, 0xfff00000fffff000L,
                                   0xffe00000fffff800L, 0xffc00000fffffc00L, 0xff800000fffffe00L, 0xff000000ffffff00L, 0xfe000000ffffff80L, 0xfc000000ffffffc0L, 0xf8000000ffffffe0L, 0xf0000000fffffff0L, 0xe0000000fffffff8L, 0xc0000000fffffffcL,
                                   0x80000000fffffffeL, 0xffffffffL, 0xfffffffe00000000L, 0xfffffffc80000000L, 0xfffffff8c0000000L, 0xfffffff0e0000000L, 0xffffffe0f0000000L, 0xffffffc0f8000000L, 0xffffff80fc000000L, 0xffffff00fe000000L,
                                   0xfffffe00ff000000L, 0xfffffc00ff800000L, 0xfffff800ffc00000L, 0xfffff000ffe00000L, 0xffffe000fff00000L, 0xffffc000fff80000L, 0xffff8000fffc0000L, 0xffff0000fffe0000L, 0xfffe0000ffff0000L, 0xfffc0000ffff8000L,
                                   0xfff80000ffffc000L, 0xfff00000ffffe000L, 0xffe00000fffff000L, 0xffc00000fffff800L, 0xff800000fffffc00L, 0xff000000fffffe00L, 0xfe000000ffffff00L, 0xfc000000ffffff80L, 0xf8000000ffffffc0L, 0xf0000000ffffffe0L,
                                   0xe0000000fffffff0L, 0xc0000000fffffff8L, 0x80000000fffffffcL, 0xfffffffeL, 0xfffffffc00000000L, 0xfffffff880000000L, 0xfffffff0c0000000L, 0xffffffe0e0000000L, 0xffffffc0f0000000L, 0xffffff80f8000000L,
                                   0xffffff00fc000000L, 0xfffffe00fe000000L, 0xfffffc00ff000000L, 0xfffff800ff800000L, 0xfffff000ffc00000L, 0xffffe000ffe00000L, 0xffffc000fff00000L, 0xffff8000fff80000L, 0xffff0000fffc0000L, 0xfffe0000fffe0000L,
                                   0xfffc0000ffff0000L, 0xfff80000ffff8000L, 0xfff00000ffffc000L, 0xffe00000ffffe000L, 0xffc00000fffff000L, 0xff800000fffff800L, 0xff000000fffffc00L, 0xfe000000fffffe00L, 0xfc000000ffffff00L, 0xf8000000ffffff80L,
                                   0xf0000000ffffffc0L, 0xe0000000ffffffe0L, 0xc0000000fffffff0L, 0x80000000fffffff8L, 0xfffffffcL, 0xfffffff800000000L, 0xfffffff080000000L, 0xffffffe0c0000000L, 0xffffffc0e0000000L, 0xffffff80f0000000L,
                                   0xffffff00f8000000L, 0xfffffe00fc000000L, 0xfffffc00fe000000L, 0xfffff800ff000000L, 0xfffff000ff800000L, 0xffffe000ffc00000L, 0xffffc000ffe00000L, 0xffff8000fff00000L, 0xffff0000fff80000L, 0xfffe0000fffc0000L,
                                   0xfffc0000fffe0000L, 0xfff80000ffff0000L, 0xfff00000ffff8000L, 0xffe00000ffffc000L, 0xffc00000ffffe000L, 0xff800000fffff000L, 0xff000000fffff800L, 0xfe000000fffffc00L, 0xfc000000fffffe00L, 0xf8000000ffffff00L,
                                   0xf0000000ffffff80L, 0xe0000000ffffffc0L, 0xc0000000ffffffe0L, 0x80000000fffffff0L, 0xfffffff8L, 0xfffffff000000000L, 0xffffffe080000000L, 0xffffffc0c0000000L, 0xffffff80e0000000L, 0xffffff00f0000000L,
                                   0xfffffe00f8000000L, 0xfffffc00fc000000L, 0xfffff800fe000000L, 0xfffff000ff000000L, 0xffffe000ff800000L, 0xffffc000ffc00000L, 0xffff8000ffe00000L, 0xffff0000fff00000L, 0xfffe0000fff80000L, 0xfffc0000fffc0000L,
                                   0xfff80000fffe0000L, 0xfff00000ffff0000L, 0xffe00000ffff8000L, 0xffc00000ffffc000L, 0xff800000ffffe000L, 0xff000000fffff000L, 0xfe000000fffff800L, 0xfc000000fffffc00L, 0xf8000000fffffe00L, 0xf0000000ffffff00L,
                                   0xe0000000ffffff80L, 0xc0000000ffffffc0L, 0x80000000ffffffe0L, 0xfffffff0L, 0xffffffe000000000L, 0xffffffc080000000L, 0xffffff80c0000000L, 0xffffff00e0000000L, 0xfffffe00f0000000L, 0xfffffc00f8000000L,
                                   0xfffff800fc000000L, 0xfffff000fe000000L, 0xffffe000ff000000L, 0xffffc000ff800000L, 0xffff8000ffc00000L, 0xffff0000ffe00000L, 0xfffe0000fff00000L, 0xfffc0000fff80000L, 0xfff80000fffc0000L, 0xfff00000fffe0000L,
                                   0xffe00000ffff0000L, 0xffc00000ffff8000L, 0xff800000ffffc000L, 0xff000000ffffe000L, 0xfe000000fffff000L, 0xfc000000fffff800L, 0xf8000000fffffc00L, 0xf0000000fffffe00L, 0xe0000000ffffff00L, 0xc0000000ffffff80L,
                                   0x80000000ffffffc0L, 0xffffffe0L, 0xffffffc000000000L, 0xffffff8080000000L, 0xffffff00c0000000L, 0xfffffe00e0000000L, 0xfffffc00f0000000L, 0xfffff800f8000000L, 0xfffff000fc000000L, 0xffffe000fe000000L,
                                   0xffffc000ff000000L, 0xffff8000ff800000L, 0xffff0000ffc00000L, 0xfffe0000ffe00000L, 0xfffc0000fff00000L, 0xfff80000fff80000L, 0xfff00000fffc0000L, 0xffe00000fffe0000L, 0xffc00000ffff0000L, 0xff800000ffff8000L,
                                   0xff000000ffffc000L, 0xfe000000ffffe000L, 0xfc000000fffff000L, 0xf8000000fffff800L, 0xf0000000fffffc00L, 0xe0000000fffffe00L, 0xc0000000ffffff00L, 0x80000000ffffff80L, 0xffffffc0L, 0xffffff8000000000L,
                                   0xffffff0080000000L, 0xfffffe00c0000000L, 0xfffffc00e0000000L, 0xfffff800f0000000L, 0xfffff000f8000000L, 0xffffe000fc000000L, 0xffffc000fe000000L, 0xffff8000ff000000L, 0xffff0000ff800000L, 0xfffe0000ffc00000L,
                                   0xfffc0000ffe00000L, 0xfff80000fff00000L, 0xfff00000fff80000L, 0xffe00000fffc0000L, 0xffc00000fffe0000L, 0xff800000ffff0000L, 0xff000000ffff8000L, 0xfe000000ffffc000L, 0xfc000000ffffe000L, 0xf8000000fffff000L,
                                   0xf0000000fffff800L, 0xe0000000fffffc00L, 0xc0000000fffffe00L, 0x80000000ffffff00L, 0xffffff80L, 0xffffff0000000000L, 0xfffffe0080000000L, 0xfffffc00c0000000L, 0xfffff800e0000000L, 0xfffff000f0000000L,
                                   0xffffe000f8000000L, 0xffffc000fc000000L, 0xffff8000fe000000L, 0xffff0000ff000000L, 0xfffe0000ff800000L, 0xfffc0000ffc00000L, 0xfff80000ffe00000L, 0xfff00000fff00000L, 0xffe00000fff80000L, 0xffc00000fffc0000L,
                                   0xff800000fffe0000L, 0xff000000ffff0000L, 0xfe000000ffff8000L, 0xfc000000ffffc000L, 0xf8000000ffffe000L, 0xf0000000fffff000L, 0xe0000000fffff800L, 0xc0000000fffffc00L, 0x80000000fffffe00L, 0xffffff00L,
                                   0xfffffe0000000000L, 0xfffffc0080000000L, 0xfffff800c0000000L, 0xfffff000e0000000L, 0xffffe000f0000000L, 0xffffc000f8000000L, 0xffff8000fc000000L, 0xffff0000fe000000L, 0xfffe0000ff000000L, 0xfffc0000ff800000L,
                                   0xfff80000ffc00000L, 0xfff00000ffe00000L, 0xffe00000fff00000L, 0xffc00000fff80000L, 0xff800000fffc0000L, 0xff000000fffe0000L, 0xfe000000ffff0000L, 0xfc000000ffff8000L, 0xf8000000ffffc000L, 0xf0000000ffffe000L,
                                   0xe0000000fffff000L, 0xc0000000fffff800L, 0x80000000fffffc00L, 0xfffffe00L, 0xfffffc0000000000L, 0xfffff80080000000L, 0xfffff000c0000000L, 0xffffe000e0000000L, 0xffffc000f0000000L, 0xffff8000f8000000L,
                                   0xffff0000fc000000L, 0xfffe0000fe000000L, 0xfffc0000ff000000L, 0xfff80000ff800000L, 0xfff00000ffc00000L, 0xffe00000ffe00000L, 0xffc00000fff00000L, 0xff800000fff80000L, 0xff000000fffc0000L, 0xfe000000fffe0000L,
                                   0xfc000000ffff0000L, 0xf8000000ffff8000L, 0xf0000000ffffc000L, 0xe0000000ffffe000L, 0xc0000000fffff000L, 0x80000000fffff800L, 0xfffffc00L, 0xfffff80000000000L, 0xfffff00080000000L, 0xffffe000c0000000L,
                                   0xffffc000e0000000L, 0xffff8000f0000000L, 0xffff0000f8000000L, 0xfffe0000fc000000L, 0xfffc0000fe000000L, 0xfff80000ff000000L, 0xfff00000ff800000L, 0xffe00000ffc00000L, 0xffc00000ffe00000L, 0xff800000fff00000L,
                                   0xff000000fff80000L, 0xfe000000fffc0000L, 0xfc000000fffe0000L, 0xf8000000ffff0000L, 0xf0000000ffff8000L, 0xe0000000ffffc000L, 0xc0000000ffffe000L, 0x80000000fffff000L, 0xfffff800L, 0xfffff00000000000L,
                                   0xffffe00080000000L, 0xffffc000c0000000L, 0xffff8000e0000000L, 0xffff0000f0000000L, 0xfffe0000f8000000L, 0xfffc0000fc000000L, 0xfff80000fe000000L, 0xfff00000ff000000L, 0xffe00000ff800000L, 0xffc00000ffc00000L,
                                   0xff800000ffe00000L, 0xff000000fff00000L, 0xfe000000fff80000L, 0xfc000000fffc0000L, 0xf8000000fffe0000L, 0xf0000000ffff0000L, 0xe0000000ffff8000L, 0xc0000000ffffc000L, 0x80000000ffffe000L, 0xfffff000L,
                                   0xffffe00000000000L, 0xffffc00080000000L, 0xffff8000c0000000L, 0xffff0000e0000000L, 0xfffe0000f0000000L, 0xfffc0000f8000000L, 0xfff80000fc000000L, 0xfff00000fe000000L, 0xffe00000ff000000L, 0xffc00000ff800000L,
                                   0xff800000ffc00000L, 0xff000000ffe00000L, 0xfe000000fff00000L, 0xfc000000fff80000L, 0xf8000000fffc0000L, 0xf0000000fffe0000L, 0xe0000000ffff0000L, 0xc0000000ffff8000L, 0x80000000ffffc000L, 0xffffe000L,
                                   0xffffc00000000000L, 0xffff800080000000L, 0xffff0000c0000000L, 0xfffe0000e0000000L, 0xfffc0000f0000000L, 0xfff80000f8000000L, 0xfff00000fc000000L, 0xffe00000fe000000L, 0xffc00000ff000000L, 0xff800000ff800000L,
                                   0xff000000ffc00000L, 0xfe000000ffe00000L, 0xfc000000fff00000L, 0xf8000000fff80000L, 0xf0000000fffc0000L, 0xe0000000fffe0000L, 0xc0000000ffff0000L, 0x80000000ffff8000L, 0xffffc000L, 0xffff800000000000L,
                                   0xffff000080000000L, 0xfffe0000c0000000L, 0xfffc0000e0000000L, 0xfff80000f0000000L, 0xfff00000f8000000L, 0xffe00000fc000000L, 0xffc00000fe000000L, 0xff800000ff000000L, 0xff000000ff800000L, 0xfe000000ffc00000L,
                                   0xfc000000ffe00000L, 0xf8000000fff00000L, 0xf0000000fff80000L, 0xe0000000fffc0000L, 0xc0000000fffe0000L, 0x80000000ffff0000L, 0xffff8000L, 0xffff000000000000L, 0xfffe000080000000L, 0xfffc0000c0000000L,
                                   0xfff80000e0000000L, 0xfff00000f0000000L, 0xffe00000f8000000L, 0xffc00000fc000000L, 0xff800000fe000000L, 0xff000000ff000000L, 0xfe000000ff800000L, 0xfc000000ffc00000L, 0xf8000000ffe00000L, 0xf0000000fff00000L,
                                   0xe0000000fff80000L, 0xc0000000fffc0000L, 0x80000000fffe0000L, 0xffff0000L, 0xfffe000000000000L, 0xfffc000080000000L, 0xfff80000c0000000L, 0xfff00000e0000000L, 0xffe00000f0000000L, 0xffc00000f8000000L,
                                   0xff800000fc000000L, 0xff000000fe000000L, 0xfe000000ff000000L, 0xfc000000ff800000L, 0xf8000000ffc00000L, 0xf0000000ffe00000L, 0xe0000000fff00000L, 0xc0000000fff80000L, 0x80000000fffc0000L, 0xfffe0000L,
                                   0xfffc000000000000L, 0xfff8000080000000L, 0xfff00000c0000000L, 0xffe00000e0000000L, 0xffc00000f0000000L, 0xff800000f8000000L, 0xff000000fc000000L, 0xfe000000fe000000L, 0xfc000000ff000000L, 0xf8000000ff800000L,
                                   0xf0000000ffc00000L, 0xe0000000ffe00000L, 0xc0000000fff00000L, 0x80000000fff80000L, 0xfffc0000L, 0xfff8000000000000L, 0xfff0000080000000L, 0xffe00000c0000000L, 0xffc00000e0000000L, 0xff800000f0000000L,
                                   0xff000000f8000000L, 0xfe000000fc000000L, 0xfc000000fe000000L, 0xf8000000ff000000L, 0xf0000000ff800000L, 0xe0000000ffc00000L, 0xc0000000ffe00000L, 0x80000000fff00000L, 0xfff80000L, 0xfff0000000000000L,
                                   0xffe0000080000000L, 0xffc00000c0000000L, 0xff800000e0000000L, 0xff000000f0000000L, 0xfe000000f8000000L, 0xfc000000fc000000L, 0xf8000000fe000000L, 0xf0000000ff000000L, 0xe0000000ff800000L, 0xc0000000ffc00000L,
                                   0x80000000ffe00000L, 0xfff00000L, 0xffe0000000000000L, 0xffc0000080000000L, 0xff800000c0000000L, 0xff000000e0000000L, 0xfe000000f0000000L, 0xfc000000f8000000L, 0xf8000000fc000000L, 0xf0000000fe000000L,
                                   0xe0000000ff000000L, 0xc0000000ff800000L, 0x80000000ffc00000L, 0xffe00000L, 0xffc0000000000000L, 0xff80000080000000L, 0xff000000c0000000L, 0xfe000000e0000000L, 0xfc000000f0000000L, 0xf8000000f8000000L,
                                   0xf0000000fc000000L, 0xe0000000fe000000L, 0xc0000000ff000000L, 0x80000000ff800000L, 0xffc00000L, 0xff80000000000000L, 0xff00000080000000L, 0xfe000000c0000000L, 0xfc000000e0000000L, 0xf8000000f0000000L,
                                   0xf0000000f8000000L, 0xe0000000fc000000L, 0xc0000000fe000000L, 0x80000000ff000000L, 0xff800000L, 0xff00000000000000L, 0xfe00000080000000L, 0xfc000000c0000000L, 0xf8000000e0000000L, 0xf0000000f0000000L,
                                   0xe0000000f8000000L, 0xc0000000fc000000L, 0x80000000fe000000L, 0xff000000L, 0xfe00000000000000L, 0xfc00000080000000L, 0xf8000000c0000000L, 0xf0000000e0000000L, 0xe0000000f0000000L, 0xc0000000f8000000L,
                                   0x80000000fc000000L, 0xfe000000L, 0xfc00000000000000L, 0xf800000080000000L, 0xf0000000c0000000L, 0xe0000000e0000000L, 0xc0000000f0000000L, 0x80000000f8000000L, 0xfc000000L, 0xf800000000000000L,
                                   0xf000000080000000L, 0xe0000000c0000000L, 0xc0000000e0000000L, 0x80000000f0000000L, 0xf8000000L, 0xf000000000000000L, 0xe000000080000000L, 0xc0000000c0000000L, 0x80000000e0000000L, 0xf0000000L,
                                   0xe000000000000000L, 0xc000000080000000L, 0x80000000c0000000L, 0xe0000000L, 0xc000000000000000L, 0x8000000080000000L, 0xc0000000L, 0x8000000000000000L, 0x80000000L, 0x0L};
};

#endif
