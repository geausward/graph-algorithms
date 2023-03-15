#include <iostream>
#include <vector> //for std::vector
#include <tuple> //for std::pair
#include <string> //for storing fileName, for reading in graph file contents
#include <fstream> //for reading in graph file contents
#include <sstream> //reading content from file
#include <algorithm> //removing vertices from adjLists
#include <cstdlib> //Utilize rand & srand functions
#include <time.h> //Seed srand function with time(0)


class edgeObj;
class vertexObj {
    public:
        /*CONSTRUCTORS*/
        vertexObj(): vertDegrees(0) {} //default no input constructor
        vertexObj(int &idInput) { //custom constructor, id input
            setVertexId(idInput);
            resetDegrees();
        }
        vertexObj(const vertexObj &copyVertex){ //copy constructor
            vertId = copyVertex.getVertId();
            connEdgeList = copyVertex.getEdgePtrs();
            vertDegrees = copyVertex.getDegrees();
        }

        /*DESTRUCTOR*/
        ~vertexObj() {}

        /*OVERLOAD OPERATORS*/
        
        vertexObj operator=(vertexObj eqVert){
            this->vertId = eqVert.getVertId();
            this->connEdgeList = eqVert.getEdgePtrs();
            this->vertDegrees = eqVert.getDegrees();
            return *this;
        }
        bool operator==(vertexObj &eqVert){
            if (this->vertId == eqVert.getVertId() && this->vertDegrees == eqVert.getDegrees()){
                return true;
            }
            return false;
        }

        /*CLASS METHODS*/
        void setVertexId(const int idInput){
            vertId = idInput;
        }
        int const getVertId() const{
            return vertId;
        }
        void addConnEdge(edgeObj* &connEdgePtr){ //adds pointer to the pointer to the adjacent vertex vector
            connEdgeList.push_back(connEdgePtr);
            vertDegrees++;
        }
        std::vector<edgeObj*> const getEdgePtrs() const{
            return connEdgeList;
        }
        //void replaceAdjEdge(vertexObj* oldVertPtr, vertexObj* newVertPtr){
        //    for (int i=0;i<connEdgeList.size();i++){
        //        if (connEdgeList[i]->getVertId()==oldVertPtr->getVertId()){
        //            connEdgeList[i]=newVertPtr;
        //        }
        //    }
        //}
        void removeConnEdge(edgeObj* &aEdgePtr){
            int index = 0, asize=0;
            do {
                asize = connEdgeList.size();
                if (connEdgeList[index] == aEdgePtr){
                    connEdgeList.erase(connEdgeList.begin()+index);
                    vertDegrees--;
                }
                else index++;
            } while (index < connEdgeList.size());
        }
        void resetAdjVerts(){
            connEdgeList.clear();
        }
        void resetDegrees(){
            vertDegrees = 0;
        }
        int const getDegrees() const {
            return vertDegrees;
        }
    private:
        int vertId; //unique ID of vertex, manually set (necessary for edge list creation from .txt file)
        int vertDegrees; //number of edges connected to this vertex
        std::vector<edgeObj*> connEdgeList; //pointers to pointers of graph vertices corresponding to adjacencies
};


class edgeObj {
    public:
        edgeObj(vertexObj* &aVert, vertexObj* &bVert)
        {
            firstVert = aVert;
            secondVert = bVert;
            toDelete = false;
        }
        ~edgeObj() {}
        vertexObj* const getFirstVert() const{
            return firstVert;
        }
        vertexObj* const getSecondVert() const{
            return secondVert;
        }
        void changeFirstVert(vertexObj* &chgFirst){
            firstVert = chgFirst;
        }
        void changeSecondVert(vertexObj* &chgSec){
            secondVert = chgSec;
        }
        bool const isVertInEdge(vertexObj* &vertA) const{
            if (firstVert == vertA){
                return true;
            }
            else if (secondVert == vertA){
                return true;
            }
            return false;
        }
    private:
        vertexObj* firstVert;
        vertexObj* secondVert;
        bool toDelete;
};


int genRandNo(const int &lowBnd=0, const int &rangeVal=1){
    //srand(time(0))
    return ((std::rand() % rangeVal) + lowBnd);
}


class graphAdjList {
    public:
        graphAdjList(): minCutVal(0) {}
        graphAdjList(std::string &fileName)
        {
            minCutVal = 0;
            minCutCalc = false;
            graphFileInput = fileName;
            readVerticesAndEdges(fileName);
        }
        graphAdjList(const graphAdjList &copyGraph)
        {
            minCutVal = 0;
            minCutCalc = false;
            vertList = copyGraph.getVertList();
            edgePtrList = copyGraph.getEdgeList();
        }
        void readVerticesAndEdges(std::string &fileName)
        {
            std::string fileLine;
            std::ifstream readVerts, readEdges;

            //Read in empty vertices per 1st column of input file
            readVerts.open(fileName);
            int vrtId = 1;
            while (getline(readVerts,fileLine))
            {
                vertexObj* newVertObj = new vertexObj(vrtId);
                vertList.push_back(newVertObj);
                vrtId++;
            }
            readVerts.close();

            //Read in edges from columns 2+ of input file
            readEdges.open(fileName);
            bool idFlag;
            int intInput;
            vertexObj *ptAVert, *ptBVert;
            while (getline(readEdges,fileLine))
            {
                idFlag = true;
                std::stringstream ss;
                ss << fileLine;
                while (ss >> intInput)
                {
                    if (!idFlag && vrtId < intInput) 
                    {
                        ptAVert = vertList[vrtId-1];
                        ptBVert = vertList[intInput-1];
                        //add logic for adding edges...
                        addEdge(ptAVert,ptBVert);
                        //Note: Still need to add ptrs of edges to vertexObjs, must complete edgePtrList first
                    }  
                    else if (idFlag) 
                    {
                        vrtId = intInput;
                        idFlag = false;
                    }
                    else if (vrtId > intInput)
                    {
                        continue;
                    }
                    else {
                        throw std::runtime_error("Improper input to file, inhibiting reading of edges");
                    }
                }
            }
            readEdges.close();

            //Add edge ptrs to vertexObjs... NOTE: TO ADD TO 'addEdge' METHOD!
            for (int k=0;k<edgePtrList.size();k++)
            {
                edgeObj* newEdgePtr = edgePtrList[k];
                edgePtrList[k]->getFirstVert()->addConnEdge(newEdgePtr);
                edgePtrList[k]->getSecondVert()->addConnEdge(newEdgePtr);
            }
        }
        ~graphAdjList() 
        {
            while (vertList.size() > 0) 
            {
                delete vertList[0];
                vertList[0] = nullptr;
                vertList.erase(vertList.begin());
            }
            while (edgePtrList.size() > 0)
            {
                delete edgePtrList[0];
                edgePtrList[0] = nullptr;
                edgePtrList.erase(edgePtrList.begin());
            }
        };

        graphAdjList operator=(graphAdjList &graphInp)
        {
            this->vertList = graphInp.getVertList();
            this->edgePtrList = graphInp.getEdgeList();
            this->minCutVal = graphInp.getMinCutVal();
            return *this;
        }

        /*CLASS METHODS*/

        void addEdge(vertexObj* &aVert, vertexObj* &bVert)
        {
            edgePtrList.push_back(new edgeObj(aVert,bVert));
        }
        void removeEdge(edgeObj* &aEdge)
        {
            aEdge->getFirstVert()->removeConnEdge(aEdge);
            aEdge->getSecondVert()->removeConnEdge(aEdge);
            bool foundEdge = false;
            for (int i=0;i<edgePtrList.size();i++){
                if (edgePtrList[i] == aEdge){
                    foundEdge=true;
                    delete edgePtrList[i];
                    edgePtrList[i] = nullptr;
                    edgePtrList.erase(edgePtrList.begin()+i);
                    break;
                }
            }
            if (!foundEdge)
            {
                throw std::runtime_error("Error: Could not find removal edge in edgePtrList.");
            }
        }
        void removeEdge(int i)
        {
            std::vector<edgeObj*> newEdgeList;
            for (int j=0;j<edgePtrList.size();j++)
            {
                if (i!=j)
                {
                    newEdgeList.push_back(edgePtrList[j]);
                }
            }
            delete edgePtrList[i];
            edgePtrList[i] = nullptr;
            edgePtrList = newEdgeList;
        }

        void resetMinCutVal()
        {
            minCutVal = 0;
        }
        int const vertexCount() const 
        {
            return vertList.size();
        }
        std::vector<vertexObj*> const getVertList() const 
        {
            return vertList;
        }
        std::vector<edgeObj*> const getEdgeList() const 
        {
            return edgePtrList;
        }
        void contractGraph()
        {
            int N = vertexCount();
            if (N < 2)
            {
                throw std::runtime_error("Error, impromper input. Expected to process a graph with >1 vertex for contracting.");
            }
            
            int thisIdx=0, contrIdx=0, contrVertId=0, contrVertDegr=0, thisVertDegr=0, randLowBound=0;
            vertexObj thisVert, contrVert;

            do {
                thisIdx = genRandNo(randLowBound,N);
                thisVertDegr = vertList[thisIdx]->getDegrees();
            } while (thisVertDegr<=0);
            //thisIdx = 0; //HARDCODED FOR DEBUG
            thisVert = *vertList[thisIdx];

            contrIdx = genRandNo(randLowBound,thisVertDegr);
            //contrIdx = 0; //HARDCODED FOR DEBUG
            if (vertList[thisIdx]->getEdgePtrs()[contrIdx]->getFirstVert()->getVertId() == vertList[thisIdx]->getVertId())
            {
                contrVert = *(vertList[thisIdx]->getEdgePtrs()[contrIdx]->getSecondVert());
            }
            else {
                contrVert = *(vertList[thisIdx]->getEdgePtrs()[contrIdx]->getFirstVert());
            }
            contrVertId = contrVert.getVertId();
            contrVertDegr = contrVert.getDegrees();

            //Find vertex to contract in main vertex list
            int findContrIdx=0;
            while (findContrIdx<vertList.size())
            {
                if(vertList[findContrIdx]->getVertId()==contrVertId)
                {
                    break;
                }
                else findContrIdx++;
            }

            // Go through edgePtrList, removing edges between thisVert and contrVert, 
            // and swaping thisVertPtr for contrVertPtr in remaining edges
            vertexObj* thisVertPtr = vertList[thisIdx];
            vertexObj* contrVertPtr = vertList[findContrIdx];
            edgeObj* edgeInQueue;
            int edgeInQueueFirstId = 0, edgeInQueueSecondId = 0;
            int edgePtrListIdx=0;
            while (edgePtrListIdx < edgePtrList.size())
            {
                edgeInQueue = edgePtrList[edgePtrListIdx];
                edgeInQueueFirstId = edgeInQueue->getFirstVert()->getVertId();
                edgeInQueueSecondId = edgeInQueue->getSecondVert()->getVertId();
                bool condA = edgeInQueue->isVertInEdge(contrVertPtr);
                bool condB = edgeInQueue->isVertInEdge(thisVertPtr);
                if (condA && condB)
                {
                    vertList[thisIdx]->removeConnEdge(edgeInQueue);
                    vertList[findContrIdx]->removeConnEdge(edgeInQueue);
                    removeEdge(edgePtrListIdx);
                }
                else if (condA && !condB) 
                {
                    if (edgeInQueueFirstId==contrVertId) 
                    {
                        edgePtrList[edgePtrListIdx]->changeFirstVert(thisVertPtr);
                    }
                    else { //edgeInQueueSecondId == contrVertId
                        edgePtrList[edgePtrListIdx]->changeSecondVert(thisVertPtr);
                    }
                    vertList[thisIdx]->addConnEdge(edgeInQueue);
                    vertList[findContrIdx]->removeConnEdge(edgeInQueue);
                    //edgePtrListIdx++;
                }
                else {edgePtrListIdx++;}
            }
            //Calculate degrees of combined vertex
            int thisVertDegrNew = vertList[thisIdx]->getDegrees();

            //Calculate new minCutVal
            int compareValMinCut;
            bool beforeCutVsAfterCut = thisVertDegr < thisVertDegrNew;
            bool beforeCutVsContrCut = thisVertDegr < contrVertDegr;
            bool contrCutVsAfterCut = contrVertDegr < thisVertDegrNew;
            if (vertList.size()!=2)
            {
                if (beforeCutVsAfterCut && beforeCutVsContrCut)
                {
                    compareValMinCut = thisVertDegr;
                }
                else if (contrCutVsAfterCut)
                {
                    compareValMinCut = contrVertDegr;
                }
                else {compareValMinCut = thisVertDegrNew;}
            }
            else {
                if (beforeCutVsContrCut)
                {
                    compareValMinCut = thisVertDegr;
                }
                else {compareValMinCut = contrVertDegr;}
            }
            if (compareValMinCut == 0)
            {
                throw std::runtime_error("Error: Calculated minCut as 0 during contraction.");
            }

            if (minCutVal == 0 || compareValMinCut < minCutVal)
            {
                minCutVal = compareValMinCut;
            }

            //Now to remove vertexObj contrVert from vertList
            vertList.erase(vertList.begin()+findContrIdx);

            // DEBUG PRINT STATEMENTS
            bool debugContraction = false;
            if (debugContraction) {
                std::cout<<"Contracting vertex "<<contrVertId<<" into vertex "<<thisVert.getVertId()<<std::endl;
                std::cout<<*this<<std::endl;
                std::cout<<"RESULTS OF CONTRACTION: "<<std::endl;
                std::cout<<"vertListSize: "<<vertList.size()<<std::endl;
                std::cout<<"edgeListSize: "<<edgePtrList.size()<<std::endl;
                std::cout<<"Contracting vertex degrees: "<<thisVertDegr<<std::endl;
                std::cout<<"Contracted vertex degrees: "<<contrVertDegr<<std::endl;
                std::cout<<"Combined vertex degrees: "<<thisVertDegrNew<<std::endl;
                std::cout<<"minCuts: "<<minCutVal<<"\n\n";
            }
        }
        void calcMinCut()
        { 
            int lenVertList = vertexCount();
            graphAdjList* minCutGraph = nullptr;
            if (!minCutCalc)
            {
                minCutCalc = true;
            }
            for (int i = 0;i<lenVertList;i++)
            {
                minCutGraph = new graphAdjList(graphFileInput);
                for (int j = 0; j<lenVertList-1;j++)
                {
                    //keep contracting graph until only one node left
                    minCutGraph->contractGraph();
                }
                if (minCutVal==0 || this->minCutVal > minCutGraph->getRawMinCut())
                {
                    this->minCutVal=minCutGraph->getRawMinCut();
                }
                std::cout<<"New Min Cut: "<<minCutVal<<std::endl;
                delete minCutGraph;
                minCutGraph = nullptr;
            }
        }
        int const getMinCutVal()
        {
            if (!minCutCalc)
            {
                calcMinCut();
            }
            return minCutVal;
        }
        int const getRawMinCut() const
        {
            return minCutVal;
        }
        friend std::ostream& operator<< (std::ostream& out, const graphAdjList& graphInp);
    private:
        std::vector<vertexObj*> vertList;
        std::vector<edgeObj*> edgePtrList;
        int minCutVal;
        bool minCutCalc;
        std::string graphFileInput;

};

std::ostream& operator<<(std::ostream& os,const graphAdjList &graphObj)
{
    os<<"PRINTING CONTENT OF GRAPH\n";
    for (int i=0;i<graphObj.vertexCount();i++)
    {
        os <<"Vertex: "<< graphObj.getVertList()[i]->getVertId() << " --->";
        if (graphObj.getVertList()[i]->getDegrees() == 0)
        {
            os <<"(no adjacent vertices)\n";
        }
        else {
            for (int j=0;j<graphObj.getVertList()[i]->getDegrees();j++)
            {
                int othVertId = graphObj.getVertList()[i]->getEdgePtrs()[j]->getFirstVert()->getVertId();
                if (othVertId == graphObj.getVertList()[i]->getVertId()){
                    othVertId = graphObj.getVertList()[i]->getEdgePtrs()[j]->getSecondVert()->getVertId();
                }
                os << " "<<othVertId;
            }
            os << "\n";
        }
    }
    os <<"(end of graph)\n";
    bool printEdges = false;
    if (printEdges) 
    {
        os <<"PRINTING EDGELIST: \n";
        for (int i=0;i<graphObj.getEdgeList().size();i++){
            os << "Edge "<<i+1<<": v1=";
            os << graphObj.getEdgeList()[i]->getFirstVert()->getVertId();
            os << "  v2=";
            os << graphObj.getEdgeList()[i]->getSecondVert()->getVertId()<<"\n";
        }
        os << "(end of edgePtrList)\n\n";
    }
    return os;
}

void graphDebug(graphAdjList &graphA)
{
    int noContractions = graphA.getVertList().size()-1;
    std::cout<<"\n"<<graphA<<std::endl;
    std::cout<<"START GRAPH CONTRACTIONS: \n\n";
    for (int j=0;j<noContractions;j++){
        graphA.contractGraph();
    }
    //std::cout<<"Minimum cuts of graph: "<<graphA.getMinCutVal()<<"\n\n\n\n";
}
int main() 
{
    std::string inFileName = "graphInput.txt";
    graphAdjList testGraph(inFileName);
    //graphDebug(testGraph);
    int minCutOutput = testGraph.getMinCutVal();
    std::cout<<"minCutOut: "<<minCutOutput<<std::endl;
    return 0;
}