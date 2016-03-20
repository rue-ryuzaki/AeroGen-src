#include "counter.h"

Counter::Counter(MCoord sz, MCoord nul_pnt): mSize(sz), mNull(nul_pnt) {
    mIsNext = true;
    mCurrent = MCoord();
}

MCoord Counter::Current() const {
    return mCurrent + mNull;
}

bool Counter::IsNext() const {
    return mIsNext;
}

void Counter::Next() {
    mIsNext = false;
    for (int d = mCurrent.GetDefDims() - 1; d >= 0; --d) {
        mCurrent.SetCoord(d, mCurrent.GetCoord(d) + 1);
        if (mCurrent.GetCoord(d) == mSize.GetCoord(d)) {
            mCurrent.SetCoord(d, 0);
        } else {
            mIsNext = true;
            break;
        }
    }
}
