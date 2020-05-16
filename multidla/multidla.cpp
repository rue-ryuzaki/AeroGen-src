#include "multidla.h"

#include <ctime>
#include <exception>
#include <iostream>
#include <set>

using namespace multidla;

template <class FLD>
uint32_t cntNeighbors(FLD* fld, const MCoordVec* mapNeigh, const MCoord& currCoord)
{
    uint32_t res = 0;
    MCoord c;
    for (size_t i = 0; i < mapNeigh->size(); ++i) {
        MCoord neighCoord = (*mapNeigh)[i];
        c = MCoord(currCoord + neighCoord);
        try {
            if (fld->isSet(c)) {
                ++res;
            }
        } catch (MOutOfBoundError& e) { }
    }
    return res;
}

MultiDLA::MultiDLA(QObject* parent)
    : Generator(parent)
{
}

MultiDLA::~MultiDLA()
{
    if (m_fld) {
        delete m_fld;
        m_fld = nullptr;
    }
}

multidla::XField* MultiDLA::field() const
{
    return m_fld;
}

void MultiDLA::generate(const Sizes& sizes, const RunParams& params)
{
    double r = 0.5385;
    MCoord::setDefDims(3);
    MCoord sz;
    double scale = params.cellSize;// * std::sqrt(1 - 0.4 * 0.4);
    sz.setCoord(0, Coordinate(sizes.x * 2.0 * r / scale));
    sz.setCoord(1, Coordinate(sizes.y * 2.0 * r / scale));
    sz.setCoord(2, Coordinate(sizes.z * 2.0 * r / scale));
    
    std::vector<double> composition;
    double porosity = 0.0;
    composition.push_back(params.porosity);
    porosity += params.porosity;
    
    if (porosity <= 0 || porosity >= 1) {
        std::cout << "Wrong porosity!" << std::endl;
        return;
    }

    m_finished = false;
    // clean up
    if (m_fld) {
        delete m_fld;
    }
#ifndef _WIN32
    uint32_t t0 = uint32_t(clock());
#endif
    m_fld = new multidla::XField(sz, MCoord(), params.cellSize);
    std::cout << "DLA" << std::endl;
    cMultiDLA(m_fld, params.porosity, params.init, params.step, params.hit);
    std::cout << "Aggregation" << std::endl;
    clusterAggregation(m_fld, params.cluster);
    for (FieldElement i = 1; i < composition.size(); ++i) {
        std::cout << "Change labels " << composition[i] << std::endl;
        changeLabels(m_fld, composition[i], i);
    }
#ifndef _WIN32
    std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    if (m_cancel) {
        QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
        std::cout << "Canceled!" << std::endl;
        m_cancel = false;
        return;
    }
    //PrintField(fld);

    // save field to file
    //fld->tofile(fileName.c_str());
    m_finished = true;
    QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
}

double MultiDLA::surfaceArea(double density, uint32_t steps) const
{
    double result = 0.0;
    if (m_finished) {
#ifndef _WIN32
        uint32_t t0 = uint32_t(clock());
#endif
        // calc
        double volume = 0.0;
        double square = 0.0;
        int32_t sx = m_fld->size().coord(0);
        int32_t sy = m_fld->size().coord(1);
        int32_t sz = m_fld->size().coord(2);
        MCoord::setDefDims(3);
        MCoord sc;
        for (size_t i = 0; i < 3; ++i) {
            sc.setCoord(i, m_fld->size().coord(i));
        }
        multidla::XField* cf = new multidla::XField(sc, MCoord(), 2 * m_fld->radius());
        double cellRadius = cf->radius();
        for (int32_t ix = 0; ix < sx; ++ix) {
            for (int32_t iy = 0; iy < sy; ++iy) {
                for (int32_t iz = 0; iz < sz; ++iz) {
                    if (m_fld->element(MCoord(ix, iy, iz)) == OCUPIED_CELL) {
                        MCoord currCoord(ix, iy, iz);
                        cf->setElement(currCoord);
                        volume += vAdd(cf, currCoord) * cube(cf->side());
                        square += SfromR(cellRadius);
                    }
                }
            }
        }
        delete cf;
        // -> Monte-Carlo
        uint32_t positive = m_fld->monteCarlo(steps);
        result = 1000000 * square * positive / (steps * density * volume);
#ifndef _WIN32
        std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    }
    return result;
}

void MultiDLA::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        int32_t sx = m_fld->size().coord(0);
        int32_t sy = m_fld->size().coord(1);
        int32_t sz = m_fld->size().coord(2);
        MCoord::setDefDims(3);
        MCoord sc;
        for (size_t i = 0; i < 3; ++i) {
            sc.setCoord(i, m_fld->size().coord(i));
        }
        multidla::XField* cf = new multidla::XField(sc, MCoord(), 2 * m_fld->radius());
        for (int32_t ix = 0; ix < sx; ++ix) {
            for (int32_t iy = 0; iy < sy; ++iy) {
                for (int32_t iz = 0; iz < sz; ++iz) {
                    if (m_fld->element(MCoord(ix, iy, iz)) == OCUPIED_CELL) {
                        MCoord currCoord(ix, iy, iz);
                        cf->setElement(currCoord);
                        volume += vAdd(cf, currCoord);
                    }
                }
            }
        }
        delete cf;
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void MultiDLA::save(const char* fileName, txt_format format) const
{
    m_fld->toFile(fileName, format);
}

void MultiDLA::load(const char* fileName, txt_format format)
{
    if (m_fld) {
        delete m_fld;
    }
    m_fld = new multidla::XField(fileName, format);
    m_finished = true;
}

uint32_t MultiDLA::random(uint32_t max) const
{
#define FAST
#undef FAST
#ifdef FAST
    uint32_t res = uint32_t(max * (((rand() - 1) & RAND_MAX) / float(RAND_MAX)));
#else
    uint32_t res;
    do {
        res = uint32_t(max * (double(rand()) / double(RAND_MAX)));
    } while (res >= max);
#endif
    return res;
}

MCoord MultiDLA::toroidizeCoords(const MCoord& coords, const MCoord& dim)
{
    return (coords + dim) % dim;
}

MCoordVec* MultiDLA::createNeighborsMap(size_t dimensions) const
{
    MCoordVec* res = new MCoordVec(2 * dimensions);
    std::vector<int32_t> vals(2 * dimensions);
    for (size_t i = 0; i < dimensions; ++i) {
        vals[i] = 1;
        vals[i + dimensions] = -1;
    }
    for (size_t num = 0; num < 2 * dimensions; ++num) {
        (*res)[num].setCoord(num % dimensions, vals[num]);
    }
    return res;
}

void MultiDLA::fillDims(MCoordVec* mapSteps, size_t currDim, MCoord& otherDims,
                        size_t dims, uint32_t step)
{
    if (currDim >= dims) {
        bool canAddAccumulatedPnt = false;
        for (uint32_t i = 0; i < dims; ++i) {
            if (otherDims.coord(i) != 0) {
                canAddAccumulatedPnt = true;
                break;
            }
        }
        if (canAddAccumulatedPnt) {
            mapSteps->push_back(otherDims);
        }
        return;
    }
    int32_t sumOfSuared = 0;
    for (size_t i = 0; i < currDim; ++i) {
        sumOfSuared += sqr(otherDims.coord(i));
    }
    int32_t remainder = sqr(int32_t(step)) - sumOfSuared;
    int32_t biggestStep = int32_t(sqrt(remainder));
    for (int32_t currStep = -biggestStep; currStep <= biggestStep; ++currStep) {
        MCoord dimsNextToFill = otherDims;
        dimsNextToFill.setCoord(currDim, currStep);
        fillDims(mapSteps, currDim + 1, dimsNextToFill, dims, step);
    }
}

MCoordVec* MultiDLA::createStepMap(size_t dims, uint32_t step)
{
    MCoordVec* mapSteps = new MCoordVec;
    
    MCoord initPnt;
    fillDims(mapSteps, 0, initPnt, dims, step);
    
    return mapSteps;
}

MCoord MultiDLA::makeStep(const MCoord& currCoord, MCoordVec* mapSteps)
{
    uint32_t st = random(uint32_t(mapSteps->size()));
    //return ToroidizeCoords(currCoord+(*mapSteps)[st]);
    return currCoord + (*mapSteps)[st];
}

MCoord MultiDLA::randomPntInFld(MCoord fldSize) const
{
    MCoord res;
    size_t dims = MCoord::defDims();
    for (size_t i = 0; i < dims; ++i) {
        res.setCoord(i, Coordinate(random(uint32_t(fldSize.coord(i)))));
    }
    return res;
}

MCoord MultiDLA::randomPntOnLiveCircle(uint32_t radius) const
{
    MCoord res;
    size_t dims = MCoord::defDims();
    int32_t currRnd;
    int32_t sum = 0;
    
    for (size_t i = 0; i < dims; ++i) {
        // TODO: Refactor this!!! floor(sqrt(sqr(radius == harsh!
        if (i != dims - 1) {
            currRnd = int32_t(random(uint32_t(floor(sqrt(sqr(int32_t(radius)) - sum)))));
        } else {
            currRnd = int32_t(floor(sqrt(sqr(int32_t(radius)) - sum)));
        }
        uint32_t directionRnd = random(2);
        for (uint32_t d = 0; d < directionRnd; ++d) {
            currRnd *= -1;
        }
        sum += sqr(currRnd);
        res.setCoord(i, currRnd);
    }
    return res;
}

bool MultiDLA::isPntOutOfRadius(const MCoord& pnt, uint32_t radius) const
{
    size_t dims = MCoord::defDims();
    Coordinate sum = 0;
    Coordinate currCoord;
    for (size_t i = 0; i < dims; ++i) {
        currCoord = pnt.coord(i);
        sum += sqr(currCoord);
    }
    return (uint32_t(sum) > sqr(radius));
}

uint32_t MultiDLA::deathRadius(uint32_t liveRadius)
{
    uint32_t multiply = 2;
    if (MCoord::defDims() == 2) {
        return uint32_t(floor(RfromS2D(multiply * SfromR2D(liveRadius))));
    } else {
        return uint32_t(floor(RfromV(multiply * VfromR(liveRadius))));
    }
}

void MultiDLA::cMultiDLA(multidla::XField* fld, double targetPorosity, uint32_t initN,
                         uint32_t step, uint32_t hitCnt)
{
    fld->clear();

    double currVol = 0.0;
    double needVol = fld->cellsCnt() * (1 - targetPorosity);
    
    bool needHit = (hitCnt != 1);
    
    size_t dimensions = MCoord::defDims();
    
    MCoordVec* mapNeigh = createNeighborsMap(dimensions);
    MCoordVec* mapSteps = createStepMap(dimensions, step);
    
    uint32_t currCnt = 0;
    MCoord currCoord;
    
    while (currCnt != initN) {
        currCoord = freeRandomPntInField(fld);
        fld->setElement(currCoord);
        currVol += vAdd(fld, currCoord);
        ++currCnt;
    }
    
    multidla::XField* hitField;
    if (needHit) {
        hitField = new multidla::XField(fld->size(), MCoord(), fld->radius() * 2);
    }
    
    while (currVol < needVol) {
        currCoord = freeRandomPntInField(fld);
        if (needHit) {
            hitField->clear();
        }
        while (true) {
            if (!fld->isSet(currCoord) && cntNeighbors(fld, mapNeigh, currCoord) != 0) {
                FieldElement newHitCnt = 1;
                if (needHit) {
                    newHitCnt = hitField->elementVal(currCoord) + 1;
                    hitField->setElementVal(currCoord, newHitCnt);
                }
                
                if (newHitCnt >= hitCnt) {
                    fld->setElement(currCoord);
                    currVol += vAdd(fld, currCoord);
                    ++currCnt;
                    break;
                }
            }
            
            MCoord tmpCoord = makeStep(currCoord, mapSteps);
            tmpCoord = toroidizeCoords(tmpCoord, fld->size());
            while (fld->isSet(tmpCoord)) {
                tmpCoord = makeStep(currCoord, mapSteps);
                tmpCoord = toroidizeCoords(tmpCoord, fld->size());
            }
            currCoord = tmpCoord;
        }
    }
    
    delete mapNeigh;
    mapNeigh = nullptr;
    delete mapSteps;
    mapSteps = nullptr;
    if (needHit) {
        delete hitField;
        hitField = nullptr;
    }
}

MCoord MultiDLA::freeRandomPntInField(multidla::XField* fld)
{
    MCoord sz = fld->size();
    MCoord res;
    while (true) {
        res = randomPntInFld(sz);
        if (!fld->isSet(res)) {
            break;
        }
    }
    return res;
}

double MultiDLA::vAdd(multidla::XField* fld, const MCoord& currCoord) const
{
    double r = 0.5385;
    double h = r - 0.5;
    double result = 0.0;
    const double sph_V = VfromR(r);
    const double dV = M_PI * h * h * (r - h / 3.0);
    uint32_t neiCnt = 0;
    static MCoordVec* mapNeigh = nullptr;
    
    if (!mapNeigh) {
        mapNeigh = createNeighborsMap(MCoord::defDims());
    }
    
    neiCnt = cntNeighbors(fld, mapNeigh, currCoord);
    result += sph_V - 2.0 * neiCnt * dV;
    return result;
}

FieldElement MultiDLA::deepestMark(std::map<FieldElement, FieldElement>& substitute,
                                   FieldElement mark)
{
    if (substitute[mark] != 0) {
        FieldElement d = deepestMark(substitute, substitute[mark]);
        substitute[mark] = d;
        return d;
    }
    return mark;
}

multidla::XField* MultiDLA::markClusters(const multidla::XField* fld)
{
    // Hoshen-Kopelman method returns field with marked clusters
    multidla::XField* result = new multidla::XField(fld->size(), fld->nullPnt(), fld->radius() * 2);
    FieldElement cluster_lbl = 0;
    size_t dims = fld->nullPnt().defDims();
    MCoordVec neigh;
    for (size_t d = dims; d != 0; --d) {
        MCoord c;
        c.setCoord(d - 1, -1);
        neigh.push_back(c);
    }
    std::map<FieldElement, FieldElement> substitute;
    for (Counter cntr(fld->size(), fld->nullPnt()); cntr.isNext(); cntr.next()) {
        if (!fld->isSet(cntr.current())) {
            continue;
        }
        std::set<FieldElement> labels;
        for (size_t d = 0; d < dims; ++d) {
            MCoord current = cntr.current() + neigh[d];
            if (!result->isElementInField(current)) {
                continue;
            }
            FieldElement lbl = result->elementVal(current);
            if (lbl != 0) {
                labels.insert(lbl);
            }
        }
        if (labels.empty()) {
            ++cluster_lbl;
            result->setElementVal(cntr.current(), cluster_lbl);
        } else {
            std::set<FieldElement>::iterator it = labels.begin();
            FieldElement min_lbl = *it;
            FieldElement deepest = deepestMark(substitute, min_lbl);

            result->setElementVal(cntr.current(), deepest);
            for (++it; it != labels.end(); ++it) {
                substitute[*it] = deepestMark(substitute, deepest);
            }
        }
    }
    for (Counter cntr(fld->size(), fld->nullPnt()); cntr.isNext(); cntr.next()) {
        if (!fld->isSet(cntr.current())) {
            continue;
        }
        FieldElement curr_lbl = result->element(cntr.current());
        if (substitute[curr_lbl] != 0) {
            result->setElementVal(cntr.current(), substitute[curr_lbl]);
        }
    }
    return result;
}

std::vector<MCoordVec>* MultiDLA::extractClusters(multidla::XField* MarkedFld)
{
    std::map<FieldElement, MCoordVec> clusters;
    for (Counter cntr(MarkedFld->size(), MarkedFld->nullPnt()); cntr.isNext(); cntr.next()) {
        FieldElement lbl = MarkedFld->elementVal(cntr.current());
        if (lbl == 0) {
            continue;
        }
        clusters[lbl].push_back(cntr.current());
        // cout << cntr.Current() << " to lbl " << lbl << endl;
    }
    std::vector<MCoordVec>* result = new std::vector<MCoordVec>(clusters.size());
    size_t i = 0;
    for (auto it = clusters.begin(); it != clusters.end(); ++it, ++i) {
        result->at(i).insert(result->at(i).begin(), it->second.begin(), it->second.end());
    }
    return result;
}

MCoordVec* MultiDLA::moveCluster(MCoordVec* cluster, MCoordVec* directions)
{
    MCoordVec* result = new MCoordVec(cluster->size());
    MCoord current_shift = directions->at(random(uint32_t(directions->size())));
    for (auto pnt = cluster->begin(), res_pnt = result->begin(); pnt != cluster->end(); ++pnt, ++res_pnt) {
        *res_pnt = *pnt + current_shift;
    }
    return result;
}

MCoordVec* MultiDLA::createDirections()
{
    size_t dims = MCoord::defDims();
    MCoordVec* result = new MCoordVec(2 * dims);
    for (size_t i = 0; i < dims; ++i) {
        result[0][i] = MCoord();
        result[0][i + dims] = MCoord();
        result->at(i).setCoord(i, -1);
        result->at(i + dims).setCoord(i, 1);
    }
    return result;
}

bool MultiDLA::isClusterInField(MCoordVec* cluster, multidla::XField* fld) const
{
    for (auto pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
        if (!fld->isElementInField(*pnt)) {
            return false;
        }
    }
    return true;
}

void MultiDLA::setClusterVal(MCoordVec* cluster, multidla::XField* fld, FieldElement value)
{
    for (auto pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
        fld->setElementVal(*pnt, value);
    }
}

void MultiDLA::removeCluster(MCoordVec* cluster, multidla::XField* fld)
{
    setClusterVal(cluster, fld, FREE_CELL);
}

void MultiDLA::restoreCluster(MCoordVec* cluster, multidla::XField* fld)
{
    setClusterVal(cluster, fld, OCUPIED_CELL);
}

bool MultiDLA::isAggregation(MCoordVec* cluster, multidla::XField* fld, MCoordVec* directions) const
{
    for (auto pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
        for (auto d = directions->begin(); d != directions->end(); ++d) {
            MCoord current_pnt = *pnt + *d;
            if (fld->isElementInField(current_pnt) && fld->isSet(current_pnt)) {
                return true;
            }
        }
    }
    return false;
}

void MultiDLA::clusterAggregation(multidla::XField* fld, size_t clusterCnt)
{
    size_t targetClusterCnt = clusterCnt;
    uint32_t maxClusterMoves = 10;

    MCoordVec* directions = createDirections();
    uint32_t iter = 0;
    uint32_t iterstep = 20;
    uint32_t maxSize = 0;
    {
        multidla::XField* marked = markClusters(fld);
        std::vector<MCoordVec>* clusters = extractClusters(marked);
        maxSize = uint32_t(clusters->size());
        if (maxSize < 1) {
            maxSize = 1;
        }
        delete marked;
        delete clusters;
    }
    while (true) {
        if (m_cancel) {
            break;
        }
        multidla::XField* marked = markClusters(fld);
        std::vector<MCoordVec>* clusters = extractClusters(marked);
        delete marked;

        std::cout << "New iter. Clusters: " << clusters->size() << std::endl;

        if (clusters->size() <= targetClusterCnt) {
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                    Q_ARG(int, std::min(100, int(100 * (maxSize - clusters->size() + targetClusterCnt)) / int(maxSize))));
            delete clusters;
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                    Q_ARG(int, std::min(100, int(100 * (maxSize - clusters->size() + targetClusterCnt)) / int(maxSize))));
            QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
        }
        
        uint32_t randomCluster = random(uint32_t(clusters->size()));

        //std::cout << "Select cluster " << random_cluster << std::endl;

        MCoordVec* cluster = new MCoordVec(clusters->at(randomCluster).begin(),
                                           clusters->at(randomCluster).end());
        removeCluster(cluster, fld);
        delete clusters;
        for (uint32_t i = 0; i < maxClusterMoves; ++i) {
            //cout << "Move " << i << endl;
            MCoordVec* newCluster = moveCluster(cluster, directions);
            if (!isClusterInField(newCluster, fld)) {
                delete newCluster;
                continue;
            }
            delete cluster;
            cluster = newCluster;
            
            if (isAggregation(newCluster, fld, directions)) {
                break;
            }
        }
        restoreCluster(cluster, fld);
        delete cluster;
    }
    delete directions;
}

void MultiDLA::printField(multidla::XField* fld) const
{
    for (int32_t i = 0; i < fld->size().coord(0); ++i) {
        for (int32_t j = 0; j < fld->size().coord(1); ++j) {
            for (int32_t k = 0; k < fld->size().coord(2); ++k) {
                std::cout << fld->element(MCoord(i, j, k)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void MultiDLA::testMarkClusters()
{
    if (MCoord::defDims() != 2) {
        MCoord::setDefDims(2);
    }
    multidla::XField fld(MCoord(5, 5), MCoord(), 1.0);
    fld.setElement(MCoord(0, 0));
    // cross
    for (int32_t row = 0; row < 3; ++row) {
        for (int32_t col = 2 - row; col <= 2 + row; ++col) {
            fld.setElement(MCoord(row, col));
            fld.setElement(MCoord(5 - row - 1, col));
        }
    }
    
    printField(&fld);
    multidla::XField* marked = markClusters(&fld);
    printField(marked);
    std::vector<MCoordVec>* clusters = extractClusters(marked);
    for (auto cluster = clusters->begin(); cluster != clusters->end(); ++cluster) {
        std::cout << "---***---" << std::endl;
        for (auto pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
            //cout << *pnt << endl;
        }
    }
    clusterAggregation(&fld);
    printField(&fld);
    delete clusters;
    delete marked;
}

MCoord MultiDLA::randomPoint(const MCoord& sz) const
{
    MCoord result;
    for (size_t i = 0; i < MCoord::defDims(); ++i) {
        result.setCoord(i, random(uint32_t(sz.coord(i))));
    }
    return result;
}

void MultiDLA::changeLabels(multidla::XField* fld, double fraction, FieldElement lbl)
{
    uint32_t cnt = uint32_t(fld->totalElements() * fraction);
    MCoord sz = fld->size();
    while (cnt > 0) {
        MCoord curr = randomPoint(sz);
        if (fld->elementVal(curr) == 1) {
            fld->setElementVal(curr, lbl);
            --cnt;
        }
    }
}
