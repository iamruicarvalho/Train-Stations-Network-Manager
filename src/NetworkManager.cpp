//
// Created by Utilizador on 12/03/2023.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include "NetworkManager.h"

using namespace std;

NetworkManager::NetworkManager() {
}

void NetworkManager::readFiles() {
    cout << "Loading files..." << endl;

    // read stations.csv
    int station_count = 0;
    std::ifstream stationsFile;
    stationsFile.open("../resources/stations.csv");
    if (!stationsFile.is_open()) {
        cout << "File not found\n";
        return;
    }
    string name, district, municipality, township, line;
    int i = 0;
    getline(stationsFile, line);
    while (getline(stationsFile, line)) {
        istringstream iss(line);
        getline(iss, name, ',');
        getline(iss, district, ',');
        getline(iss, municipality, ',');
        getline(iss, township, ',');
        getline(iss, line, '\0');

        Station station(name, district, municipality, township, line);
        stationsSet.insert(station);

        if (!(stations_code_reverse.count(name)) && name != "NAME") {
            i++;
            stations_code_reverse[name] = i;
            stations_code[i] = name;
            addVertex(i);
        }
    }
    stationsFile.close();
    cout << "There are " << stationsSet.size() << " stations!" << endl;


    // read network.csv
    int network_count = 0;
    std::ifstream networkFile;
    networkFile.open("../resources/network.csv");
    if (!networkFile.is_open()) {
        cout << "File not found\n";
        return;
    }

    // getline(networkFile, line);
    while (getline(networkFile, line)) {
        //row.clear();
        string stationA, stationB, service;
        double capacity;
        istringstream iss(line);
        getline(iss, stationA, ',');
        getline(iss, stationB, ',');
        iss >> capacity;
        iss.ignore(1);
        getline(iss, service, '\0');

        Network network(stationA, stationB, capacity, service);
        networkSet.insert(network);

        int code_StationA = stations_code_reverse[stationA];
        int code_StationB = stations_code_reverse[stationB];
        railway.addEdge(code_StationA, code_StationB, capacity);
    }
    networkFile.close();
    cout << "In all, there are " << networkSet.size() << " possible connections in the provided railway network!" << endl;
}


//Bloquear as linhas atraves da string
//retorna um set com os ids de cada station que fica cortada por causa da linha que usa
set<int> NetworkManager::returnBlockedStations(const string &blockLine) {
    set<int> result;
    for (auto &c: stationsSet) {
        if (c.getLine() == blockLine) {
            result.insert(stations_code_reverse[c.getName()]);
        }
    }
    return result;
}

void NetworkManager::setBlockLine(const string &blockline) {
    set<int> blockeds = returnBlockedStations(blockline);
    for (auto &c: blockeds) {
        for (auto d: vertexSet) {
            if (c == d->getId()) {
                d->setBlock();
            }
        }
    }
}

//2.1

bool NetworkManager::augmentingPath(int source, int target) {
    for (auto vertex: vertexSet) {
        vertex->setVisited(false);
        vertex->setPath(nullptr);
    }
    std::queue<Vertex *> q;
    Vertex *starting = findVertex(source);
    q.push(starting);
    while (!q.empty()) {
        Vertex *that = q.front();
        q.pop();
        for (auto edge: that->getAdj()) {
            edge->setSelected(false);
            Vertex *dest = edge->getDest();
            if (!dest->isVisited() && edge->getWeight() - edge->getFlow() > 0 && !that->isBlocked()) {
                dest->setVisited(true);
                dest->setPath(edge);
                q.push(dest);
            }
        }
        for (auto edge: that->getIncoming()) {
            Vertex *incoming = edge->getOrig();
            if (!incoming->isVisited() && edge->getFlow() > 0 && !incoming->isBlocked()) {
                incoming->setVisited(true);
                Edge *reversed = new Edge(edge->getDest(), edge->getOrig(), edge->getWeight());
                edge->setReverse(reversed);
                incoming->setPath(edge);
                q.push(incoming);
            }
        }
    }
    return findVertex(target)->isVisited();
}

int NetworkManager::minResidual(int source, int target) {
    int disponivel;
    Vertex *end = findVertex(target);
    Vertex *incoming = end->getPath()->getOrig();
    if (incoming->getPath() == nullptr)
        return end->getPath()->getWeight();

    int minFlow = end->getPath()->getWeight() - incoming->getPath()->getFlow();

    while (incoming->getId() != source) {
        if (incoming->getPath()->getReverse() == nullptr) {
            disponivel = incoming->getPath()->getWeight() - incoming->getPath()->getFlow();
            minFlow = std::min(minFlow, disponivel);
            incoming = incoming->getPath()->getOrig();
        } else {
            disponivel = incoming->getPath()->getFlow();
            minFlow = std::min(minFlow, disponivel);
            incoming = incoming->getPath()->getDest();
        }
    }
    return minFlow;
}

void NetworkManager::update(int flow, int source, int target) {
    Vertex *u = findVertex(target);
    while (u != findVertex(source)) {
        auto edge = u->getPath();
        if (edge->getReverse() == nullptr) {
            edge->setFlow(edge->getFlow() + flow);
            u = edge->getOrig();
        } else {
            edge->setFlow(edge->getFlow() - flow);
            u = edge->getDest();
            edge->setReverse(nullptr);
        }
    }
}

int NetworkManager::max_trains(string A, string B, bool changed) {
    int source = stations_code_reverse[A];
    int target = stations_code_reverse[B];
    if(source == 0 | target==0) return -1;
    int result_final = 0;
    for (auto vertex: vertexSet) {
        for (auto edge: vertex->getAdj()) {
            edge->setFlow(0);
        }
    }
    while (augmentingPath(source, target)) {
        int flow = minResidual(source, target);
        update(flow, source, target);
        result_final += flow;
    }
    if (result_final == 0 && changed)
        return max_trains(B, A, false); // caso as estações source e target estejam trocadas, corre-se o codigo novamente, com as estações trocadas
    else
        return result_final;
}

