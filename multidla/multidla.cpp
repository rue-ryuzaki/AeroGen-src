#include "multidla.h"

#include <iostream>
#include <math.h>
#include <time.h>
#include <exception>
#include <set>
#include <QFile>

#define DIMS 2
#define INIT_RADIUS 20

MultiDLA::MultiDLA(QObject * parent)
    : Generator(parent)
{
}

MultiDLA::~MultiDLA()
{
}

CellsField * MultiDLA::GetField() const
{
    return fld;
}

void MultiDLA::Generate(const Sizes & sizes, double por, int initial, int step,
        int hit, size_t cluster, double cellsize)
{
    //srand((unsigned)time(0));
    double r = 0.5385;
    MCoord::SetDefDims(3);
    MCoord sz;
    double scale = cellsize;// * pow((1 - 0.4 * 0.4), 0.5);
    sz.SetCoord(0, sizes.x * 2 * r / scale);
    sz.SetCoord(1, sizes.y * 2 * r / scale);
    sz.SetCoord(2, sizes.z * 2 * r / scale);
    
    std::vector<double> composition;
    double porosity = 0.0;
    composition.push_back(por);
    porosity += por;
    
    if (porosity <= 0 ||porosity >= 1) {
        std::cout << "Wrong porosity!" << std::endl;
        return;
    }
    
    int initCnt = initial;
    
    int stepLen = step;
    
    int hitCnt = hit;
    
    size_t cluster_cnt = cluster;
    finished = false;
    if (calculated) {
        // clean up
        delete fld;
        calculated = false;
    }
    fld = new CellsField(sz, MCoord(), cellsize);
    calculated = true;
    std::cout << "DLA" << std::endl;
    cMultiDLA(fld, porosity, initCnt, stepLen, hitCnt);
    std::cout << "Aggregation" << std::endl;
    ClusterAggregation(fld, cluster_cnt);
    for (uint i = 1; i < composition.size(); ++i) {
        std::cout << "Change labels " << composition[i] << std::endl;
        ChangeLabels(fld, composition[i], i);
    }
    if (cancel) {
        QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
        std::cout << "Canceled!" << std::endl;
        cancel = false;
        return;
    }
    //PrintField(fld);

    // save field to file
    //fld->tofile(fileName.c_str());
    finished = true;
    QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
    
    // clean up
    //delete fld;
}

double MultiDLA::SurfaceArea(double density)
{
    double result = 0.0;
    if (finished) {
        // calc
        double volume = 0.0;
        double square = 0.0;
        int sx = fld->GetSize().GetCoord(0);
        int sy = fld->GetSize().GetCoord(1);
        int sz = fld->GetSize().GetCoord(2);
        MCoord::SetDefDims(3);
        MCoord sc;
        for (int i = 0; i < 3; ++i) {
            sc.SetCoord(i, fld->GetSize().GetCoord(i));
        }
        CellsField * cf = new CellsField(sc, MCoord(), 2 * fld->getRadius());
        double cellRadius = cf->getRadius();
        for (int ix = 0; ix < sx; ++ix) {
            for (int iy = 0; iy < sy; ++iy) {
                for (int iz = 0; iz < sz; ++iz) {
                    if (fld->GetElement(MCoord(ix, iy, iz)) == OCUPIED_CELL) {
                        MCoord currCoord(ix, iy, iz);
                        cf->SetElement(currCoord);
                        volume += V_add(cf, currCoord) * cube(cf->getSide());
                        square += SfromR(cellRadius);
                    }
                }
            }
        }
        delete cf;
        // -> Monte-Carlo
        int stepMax = 5000;
        int positive = fld->MonteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void MultiDLA::Density(double density, double & denAero, double & porosity)
{
    if (finished) {
        // calc
        double volume = 0.0;
        int sx = fld->GetSize().GetCoord(0);
        int sy = fld->GetSize().GetCoord(1);
        int sz = fld->GetSize().GetCoord(2);
        MCoord::SetDefDims(3);
        MCoord sc;
        for (int i = 0; i < 3; ++i) {
            sc.SetCoord(i, fld->GetSize().GetCoord(i));
        }
        CellsField * cf = new CellsField(sc, MCoord(), 2 * fld->getRadius());

        for (int ix = 0; ix < sx; ++ix) {
            for (int iy = 0; iy < sy; ++iy) {
                for (int iz = 0; iz < sz; ++iz) {
                    if (fld->GetElement(MCoord(ix, iy, iz)) == OCUPIED_CELL) {
                        MCoord currCoord(ix, iy, iz);
                        cf->SetElement(currCoord);
                        volume += V_add(cf, currCoord);
                    }
                }
            }
        }
        delete cf;
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void MultiDLA::Save(const char * fileName, txt_format format) const
{
    fld->toFile(fileName, format);
}

void MultiDLA::Save(std::string fileName, txt_format format) const
{
    fld->toFile(fileName.c_str(), format);
}

void MultiDLA::Load(const char * fileName, txt_format format)
{
    if (fld != nullptr) {
        delete fld;
    }
    fld = new CellsField(fileName, format);
    finished = true;
}

void MultiDLA::Load(std::string fileName, txt_format format)
{
    if (fld != nullptr) {
        delete fld;
    }
    fld = new CellsField(fileName.c_str(), format);
    finished = true;
}

int MultiDLA::random(int max)
{
#define FAST
#undef FAST
#ifdef FAST
    int res = int(max * (((rand() - 1) & RAND_MAX) / (float)(RAND_MAX)));
#else
    int res;
    do {
        res = int(max * (rand() / (float)(RAND_MAX)));
    } while (res >= max);
#endif
    return res;
}

MCoord MultiDLA::ToroidizeCoords(const MCoord & coords, const MCoord & dim)
{
    return (coords + dim) % dim;
}

MCoordVec * MultiDLA::CreateNeighborsMap(int dimensions)
{
    MCoordVec *res = new MCoordVec(2 * dimensions);
    std::vector<int> vals(2 * dimensions);
    for (int i = 0; i < dimensions; ++i) {
        vals[i] = 1;
        vals[i + dimensions] = -1;
    }
    for (int num = 0; num < 2 * dimensions; ++num) {
        (*res)[num].SetCoord(num % dimensions, vals[num]);
    }
    return res;
}

void MultiDLA::FillDims(MCoordVec * mapSteps, int currDim, MCoord & otherDims, int dims, int step)
{
    if (currDim >= dims) {
        bool canAddAccumulatedPnt = false;
        for (int i = 0; i < dims; ++i) {
            if (otherDims.GetCoord(i) != 0) {
                canAddAccumulatedPnt = true;
                break;
            }
        }
        if (canAddAccumulatedPnt) {
            mapSteps->push_back(otherDims);
        }
        return;
    }
    int sumOfSuared = 0;
    for (int i = 0; i < currDim; ++i) {
        sumOfSuared += sqr(otherDims.GetCoord(i));
    }
    int remainder = sqr(step) - sumOfSuared;
    int biggestStep = int(sqrt(remainder));
    for (int currStep = -biggestStep; currStep <= biggestStep; ++currStep) {
        MCoord dimsNextToFill = otherDims;
        dimsNextToFill.SetCoord(currDim, currStep);
        FillDims(mapSteps, currDim + 1, dimsNextToFill, dims, step);
    }
}

MCoordVec * MultiDLA::CreateStepMap(int dims, int step)
{
    MCoordVec * mapSteps = new MCoordVec;
    
    MCoord initPnt;
    FillDims(mapSteps, 0, initPnt, dims, step);
    
    return mapSteps;
}

MCoord MultiDLA::MakeStep(const MCoord & currCoord, MCoordVec * mapSteps)
{
    int st = random((int)mapSteps->size());
    //return ToroidizeCoords(currCoord+(*mapSteps)[st]);
    return currCoord + (*mapSteps)[st];
}

MCoord MultiDLA::RandomPntInFld(MCoord fldSize)
{
    MCoord res;
    int dims = (int)MCoord::GetDefDims();
    for (int i = 0; i < dims; ++i) {
        res.SetCoord(i, random(fldSize.GetCoord(i)));
    }
    return res;
}

MCoord MultiDLA::RandomPntOnLiveCircle(int radius)
{
    MCoord res;
    size_t dims = MCoord::GetDefDims();
    int currRnd;
    int sum = 0;
    
    for (size_t i = 0; i < dims; ++i) {
        // TODO: Refactor this!!! floor(sqrt(sqr(radius == harsh!
        if (i != dims - 1) {
            currRnd = random(floor(sqrt(sqr(radius) - sum)));
        } else {
            currRnd = floor(sqrt(sqr(radius) - sum));
        }
        int directionRnd = random(2);
        for (int d = 0; d < directionRnd; ++d) {
            currRnd *= -1;
        }
        sum += sqr(currRnd);
        res.SetCoord(i, currRnd);
    }
    return res;
}

bool MultiDLA::IsPntOutOfRadius(const MCoord & pnt, int radius)
{
    size_t dims = MCoord::GetDefDims();
    Coordinate sum = 0;
    Coordinate currCoord;
    for (size_t i = 0; i < dims; ++i) {
        currCoord = pnt.GetCoord(i);
        sum += sqr(currCoord);
    }
    return (sum > sqr(radius));
}

int MultiDLA::GetDeathRadius(int liveRadius)
{
    int multiply = 2;
    if (MCoord::GetDefDims() == 2) {
        return floor(RfromS2D(multiply * SfromR2D(liveRadius)));
    } else {
        return floor(RfromV(multiply * VfromR(liveRadius)));
    }
}

void MultiDLA::cMultiDLA(CellsField *fld, double targetPorosity, int initN, int step, int hitCnt)
{
    fld->Clear();
//  if (N < 1) {
//      fld->Clear();
//      return;
//  }
    double currVol = 0.0;
    double needVol = fld->GetCellsCnt() * (1 - targetPorosity);
    
    bool needHit = (hitCnt != 1);
    
    int dimensions = (int)MCoord::GetDefDims();
    
    MCoordVec * mapNeigh = CreateNeighborsMap(dimensions);
    MCoordVec * mapSteps = CreateStepMap(dimensions, step);
    
    int currCnt = 0;
    MCoord currCoord;
    
    while (currCnt != initN) {
        currCoord = FreeRandomPntInField(fld);
        fld->SetElement(currCoord);
        currVol += V_add(fld, currCoord);
        ++currCnt;
    }
    
    CellsField * hitField;
    if (needHit) {
        hitField = new CellsField(fld->GetSize(), MCoord(), fld->getRadius() * 2);
    }
    
    while (currVol < needVol) {
        currCoord = FreeRandomPntInField(fld);
        if (needHit) {
            hitField->Clear();
        }
        while (true) {
            if (!fld->IsSet(currCoord) && CntNeighbors(fld, mapNeigh, currCoord) != 0) {
                int newHitCnt = 1;
                if (needHit) {
                    newHitCnt = hitField->GetElementVal(currCoord) + 1;
                    hitField->SetElementVal(currCoord, newHitCnt);
                }
                
                if (newHitCnt >= hitCnt) {
                    fld->SetElement(currCoord);
                    currVol += V_add(fld, currCoord);
                    ++currCnt;
                    break;
                }
            }
            
            MCoord tmpCoord = MakeStep(currCoord, mapSteps);
            tmpCoord = ToroidizeCoords(tmpCoord, fld->GetSize());
            while (fld->IsSet(tmpCoord)) {
                tmpCoord = MakeStep(currCoord, mapSteps);
                tmpCoord = ToroidizeCoords(tmpCoord, fld->GetSize());
            }
            currCoord = tmpCoord;
        }
    }
    
    delete mapNeigh;
    mapNeigh = 0;
    delete mapSteps;
    mapSteps = 0;
    if (needHit) {
        delete hitField;
    }
    hitField = 0;
}

MCoord MultiDLA::FreeRandomPntInField(CellsField * fld)
{
    MCoord sz = fld->GetSize();
    MCoord res;
    while (true) {
        res = RandomPntInFld(sz);
        if (!fld->IsSet(res)) {
            break;
        }
    }
    return res;
}

double MultiDLA::V_add(CellsField * fld, const MCoord & currCoord)
{
    double r = 0.5385;
    double h = r - 0.5;
    double result = 0.0;
    const double sph_V = VfromR(r);
    const double dV = M_PI * h * h * (r - h / 3);
    int neiCnt = 0;
    static MCoordVec * mapNeigh = NULL;
    
    if (mapNeigh == NULL) {
        int dimensions = (int)MCoord::GetDefDims();
        mapNeigh = CreateNeighborsMap(dimensions);
    }
    
    neiCnt = CntNeighbors(fld, mapNeigh, currCoord);
    result += sph_V - 2 * neiCnt * dV;
    return result;
}

FieldElement MultiDLA::GetDeepestMark(std::map<FieldElement, FieldElement> & substitute, FieldElement mark)
{
    if (substitute[mark] != 0) {
        FieldElement d = GetDeepestMark(substitute, substitute[mark]);
        substitute[mark] = d;
        return d;
    }
    return mark;
}

CellsField * MultiDLA::MarkClusters(const CellsField * fld)
{
    // Hoshen-Kopelman method returns field with marked clusters
    CellsField * result = new CellsField(fld->GetSize(), fld->GetNullPnt(), fld->getRadius() * 2);
    FieldElement cluster_lbl = 0;
    size_t dims = fld->GetNullPnt().GetDefDims();
    MCoordVec neigh;
    for (size_t d = dims; d != 0; --d) {
        MCoord c;
        c.SetCoord(d - 1, -1);
        neigh.push_back(c);
    }
    std::map<FieldElement, FieldElement> substitute;
    for (Counter cntr(fld->GetSize(), fld->GetNullPnt()); cntr.IsNext(); cntr.Next()) {
        if (!fld->IsSet(cntr.Current())) {
            continue;
        }
        std::set<FieldElement> labels;
        for (size_t d = 0; d < dims; ++d) {
            MCoord current = cntr.Current() + neigh[d];
            if (!result->IsElementInField(current)) {
                continue;
            }
            FieldElement lbl = result->GetElementVal(current);
            if (lbl != 0) {
                labels.insert(lbl);
            }
        }
        if (labels.empty()) {
            ++cluster_lbl;
            result->SetElementVal(cntr.Current(), cluster_lbl);
        } else {
            std::set<FieldElement>::iterator it = labels.begin();
            FieldElement min_lbl = *it;
            FieldElement deepest = GetDeepestMark(substitute, min_lbl);

            result->SetElementVal(cntr.Current(), deepest);
            for (++it; it != labels.end(); ++it) {
                substitute[*it] = GetDeepestMark(substitute, deepest);
            }
        }
    }
    for (Counter cntr(fld->GetSize(), fld->GetNullPnt()); cntr.IsNext(); cntr.Next()) {
        if (!fld->IsSet(cntr.Current())) {
            continue;
        }
        FieldElement curr_lbl = result->GetElement(cntr.Current());
        if (substitute[curr_lbl] != 0) {
            result->SetElementVal(cntr.Current(), substitute[curr_lbl]);
        }
    }
    return result;
}

std::vector<MCoordVec> * MultiDLA::ExtractClusters(CellsField * MarkedFld)
{
    std::map<FieldElement, MCoordVec> clusters;
    for (Counter cntr(MarkedFld->GetSize(), MarkedFld->GetNullPnt()); cntr.IsNext(); cntr.Next()) {
        FieldElement lbl = MarkedFld->GetElementVal(cntr.Current());
        if (lbl == 0) {
            continue;
        }
        clusters[lbl].push_back(cntr.Current());
        // cout << cntr.Current() << " to lbl " << lbl << endl;
    }
    std::vector<MCoordVec> * result = new std::vector<MCoordVec>(clusters.size());
    int i = 0;
    for (std::map<FieldElement, MCoordVec>::iterator it = clusters.begin(); it != clusters.end(); ++it, ++i) {
        result->at(i).insert(result->at(i).begin(), it->second.begin(), it->second.end());
    }
    return result;
}

MCoordVec * MultiDLA::MoveCluster(MCoordVec * cluster, MCoordVec * directions)
{
    MCoordVec * result = new MCoordVec(cluster->size());
    MCoord current_shift = directions->at(random(directions->size()));
    for (MCoordVec::iterator pnt = cluster->begin(), res_pnt = result->begin(); pnt != cluster->end(); ++pnt, ++res_pnt) {
        *res_pnt = *pnt + current_shift;
    }
    return result;
}

MCoordVec * MultiDLA::CreateDirections()
{
    MCoordVec * result = new MCoordVec(2 * MCoord::GetDefDims());
    for (size_t i = 0; i < MCoord::GetDefDims(); ++i) {
    	result[0][i] = MCoord();
    	result[0][i + MCoord::GetDefDims()] = MCoord();
        result->at(i).SetCoord(i, -1);
        result->at(i + MCoord::GetDefDims()).SetCoord(i, 1);
    }
    return result;
}

bool MultiDLA::IsClusterInField(MCoordVec * cluster, CellsField * fld)
{
    for (MCoordVec::iterator pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
        if (!fld->IsElementInField(*pnt)) {
            return false;
        }
    }
    return true;
}

void MultiDLA::SetClusterVal(MCoordVec * cluster, CellsField * fld, FieldElement value)
{
    for (MCoordVec::iterator pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
        fld->SetElementVal(*pnt, value);
    }
}

void MultiDLA::RemoveCluster(MCoordVec * cluster, CellsField * fld)
{
    SetClusterVal(cluster, fld, FREE_CELL);
}

void MultiDLA::RestoreCluster(MCoordVec * cluster, CellsField * fld)
{
    SetClusterVal(cluster, fld, OCUPIED_CELL);
}

bool MultiDLA::IsAggregation(MCoordVec * cluster, CellsField * fld, MCoordVec * directions) {
    for (MCoordVec::iterator pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
        for (MCoordVec::iterator d = directions->begin(); d != directions->end(); ++d) {
            MCoord current_pnt = *pnt + *d;
            if (fld->IsElementInField(current_pnt) && fld->IsSet(current_pnt)) {
                return true;
            }
        }
    }
    return false;
}

void MultiDLA::ClusterAggregation(CellsField * fld, size_t cluster_cnt)
{
    size_t target_cluster_cnt = cluster_cnt;
    int max_cluster_moves = 10;

    MCoordVec * directions = CreateDirections();
    int iter = 0;
    int iterstep = 20;
    int maxSize = 0;
    {
        CellsField * marked = MarkClusters(fld);
        std::vector<MCoordVec> * clusters = ExtractClusters(marked);
        maxSize = clusters->size();
        if (maxSize < 1) maxSize = 1;
        delete marked;
        delete clusters;
    }
    while (true) {
        if (cancel) {
            break;
        }
        CellsField * marked = MarkClusters(fld);
        std::vector<MCoordVec> * clusters = ExtractClusters(marked);
        delete marked;

        std::cout << "New iter. Clusters: " << clusters->size() << std::endl;

        QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusters->size() + target_cluster_cnt)) / maxSize)));
        
        if (clusters->size() <= target_cluster_cnt) {
            delete clusters;
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
        }
        
        int random_cluster = random(clusters->size());

        //std::cout << "Select cluster " << random_cluster << std::endl;

        MCoordVec * cluster = new MCoordVec(clusters->at(random_cluster).begin(), clusters->at(random_cluster).end());
        RemoveCluster(cluster, fld);
        delete clusters;
        for (int i = 0; i < max_cluster_moves; ++i) {
            //cout << "Move " << i << endl;
            MCoordVec * new_cluster = MoveCluster(cluster, directions);
            if (!IsClusterInField(new_cluster, fld)) {
                delete new_cluster;
                continue;
            }
            delete cluster;
            cluster = new_cluster;
            
            if (IsAggregation(new_cluster, fld, directions)) {
                break;
            }
        }
        RestoreCluster(cluster, fld);
        delete cluster;
    }
    delete directions;
}

void MultiDLA::PrintField(CellsField * fld)
{
    for (int i = 0; i < fld->GetSize().GetCoord(0); ++i) {
        for (int j = 0; j < fld->GetSize().GetCoord(1); ++j) {
            for (int k = 0; k < fld->GetSize().GetCoord(2); ++k) {
                std::cout << fld->GetElement(MCoord(i, j, k)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void MultiDLA::TestMarkClusters()
{
    if (MCoord::GetDefDims() != 2) {
        MCoord::SetDefDims(2);
    }
    CellsField fld(MCoord(5, 5), MCoord(), 1.0);
    fld.SetElement(MCoord(0, 0));
    // cross
    for (int row = 0; row < 3; ++row) {
        for (int col = 2 - row; col <= 2 + row; ++col) {
            fld.SetElement(MCoord(row, col));
            fld.SetElement(MCoord(5 - row - 1, col));
        }
    }
    
    PrintField(&fld);
    CellsField * marked = MarkClusters(&fld);
    PrintField(marked);
    std::vector<MCoordVec> * clusters = ExtractClusters(marked);
    for (std::vector<MCoordVec>::iterator cluster = clusters->begin(); cluster != clusters->end(); ++cluster) {
        std::cout << "---***---" << std::endl;
        for (MCoordVec::iterator pnt = cluster->begin(); pnt != cluster->end(); ++pnt) {
            //cout << *pnt << endl;
        }
    }
    ClusterAggregation(&fld);
    PrintField(&fld);
    delete clusters;
    delete marked;
}

MCoord MultiDLA::RandomPoint(const MCoord & sz)
{
    MCoord result;
    for (uint i = 0; i < MCoord::GetDefDims(); ++i) {
        result.SetCoord(i, random(sz.GetCoord(i)));
    }
    return result;
}

void MultiDLA::ChangeLabels(CellsField * fld, double fraction, FieldElement lbl)
{
    int cnt = int(fld->GetTotalElements() * fraction);
    MCoord sz = fld->GetSize();
    while (cnt > 0) {
        MCoord curr = RandomPoint(sz);
        if (fld->GetElementVal(curr) == 1) {
            fld->SetElementVal(curr, lbl);
            --cnt;
        }
    }
}
