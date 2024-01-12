#include "ToyPass.h"

bool ToyPassImpl::run() {
    bool MadeChange = false;

    std::vector<Loop *> OriginalLoops;
    for (Loop *Outermost : LI) {
        OriginalLoops.push_back(Outermost);
    }

    for (Loop *Outermost : OriginalLoops) {
        for (Loop *L : post_order(Outermost)) {
            MadeChange |= runOnLoop(L);
        }
    }

    return MadeChange;
}   

bool ToyPassImpl::runOnLoop(Loop *L) {
    PrefetchLoopTransform PFTF(LI, SE, DT, AC, TI, TTI, ORE, MSSAU);
    return PFTF.run(L, true, true, 3, 2);
}