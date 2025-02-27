

#include <iostream>
#include "src/NetworkManager.h"
#include <locale.h>
#include <codecvt>

/**
 * Função que mostra as funcionalidades do programa
 */
void showMenu() {
    cout << "1 - Listar estações." << endl;
    cout << "2 - Ver ligações entre estações." << endl;
    cout << "3 - Pretendo saber o fluxo maximo de comboios entre 2 estações." << endl;
    cout << "4 - Pretendo saber qual é o máximo valor possível de fluxo entre 2 estações quaisquer." << endl;
    cout << "5 - Pretendo ver onde é necessário mais comboios." << endl;
    cout << "6 - Desejo ver quantos comboios podem chegar ao mesmo tempo a uma estação." << endl;
    cout << "7 - Pretendo saber o fluxo maximo de comboios entre 2 estações, tendo em conta os custos." << endl;
    cout << "8 - Desejo reportar um troço do caminho de ferro que não está operacional." << endl;
    cout << "9 - Desejo reportar um troço do caminho de ferro que já está operacional." << endl;
    cout << "10 - Pretendo saber o fluxo maximo de comboios entre 2 estações, tendo em conta as linhas que foram cortadas." << endl;
    cout << "11 - Pretendo saber quais as estações mais afetadas por cada bloqueio da ferrovia." <<endl;
    cout << "12 - Pretendo saber se existe um caminho entre duas estações." << endl;
    cout << "0 - Sair" << endl;
}

int main() {
    std::locale::global(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    setlocale(LC_ALL, "Portuguese");

    NetworkManager networkManager;
    cout << "[LOADING FILES]" << endl;
    networkManager.readFiles();

    bool running = true;
    while (running) {
        showMenu();
        int input;
        cin >> input;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(256, '\n');
            int option = 0;
        }
        switch (input) {
            case 1: {
                networkManager.listStations();
                cout << endl;
                break;
            }
            case 2: {
                networkManager.listConnections();
                cout << endl;
                break;
            }
            case 3: {
                string first, second;
                cout << "Qual é a primeira estação?" << endl;
                cin.ignore();
                getline(cin, first);
                cout << "Qual é a segunda estacao?" << endl;
                getline(cin, second);
                int result = networkManager.max_trains(first, second);
                if (result == -1) {
                    cout << "Erro ao ler as estações submetidas.\n" << endl;
                } else if (result == 0) { // quando dá 0 significa que não existe um caminho entre as duas estações
                    cout << "Nao existe um caminho entre essas 2 estações.\n" << endl;
                } else {
                    cout << "No máximo, entre essas duas estacoes, podem passar " << result << " comboios ao mesmo tempo.\n"
                         << endl;
                }
                break;
            }
            case 4: {
                vector<pair<int,pair<string,string>>> total= networkManager.max_of_max_trains();
                for(auto result:total){
                    cout << "entre " << result.second.first << " e " <<result.second.second<< ", ";
                }
                cout << "que corresponde(m) à(s) ligação(ões) com maior fluxo possível, podem passar "<<
                                    total[0].first <<" comboios ao mesmo tempo.\n" << endl;
                break;
            }
            case 5:
                cout << "1 - Pretendo saber qual o municipio que necessita de mais comboios." << endl;
                cout << "2 - Pretendo saber qual o distrito que necessita de mais comboios." << endl;
                int n;
                int k;
                cin >> n;
                cout << "Digite o top-k que deseja ver" <<endl;
                cin >> k;
                switch (n) {
                    case 1:
                        networkManager.trainManagementByMunicipality(k);
                        cout << endl;
                        break;
                    case 2:
                        networkManager.trainManagementByDistrict(k);
                        cout << endl;
                        break;
                }
                break;
            case 6: {
                string station;
                cout << "Qual é a estação?" << endl;
                cin.ignore();
                getline(cin,station);
                int result = networkManager.maxTrainsArrivingAtStation(station);
                cout << result << endl << endl;
                break;
            }
            case 7: {
                string first, second;
                cout << "Qual é a primeira estação?" << endl;
                cin.ignore();
                getline(cin,first);
                cout << "Qual é a segunda estação?" << endl;
                getline(cin,second);
                networkManager.dijkstra(first,second);
                cout << endl << endl;
                break;
            }
            case 8: {
                string first, second;
                cout << "Qual é a primeira estação do troço que deseja cortar?" << endl;
                cin.ignore();
                getline(cin, first);
                cout << "Qual é a segunda estação do troço que deseja cortar?" << endl;
                getline(cin, second);
                bool found= networkManager.set_block(first,second);
                if(found){
                    cout<< "Bloqueio feito entre essas 2 estações com sucesso.\n" << endl;
                }
                else{
                    cout<< "Não foi encontrado um troço que conecte diretamente essas 2 estações.\n" << endl;
                }
                break;
            }
            case 9: {
                string first, second;
                cout << "Qual é a primeira estação do bloqueio que deseja remover?" << endl;
                cin.ignore();
                getline(cin, first);
                cout << "Qual é a segunda estação do bloqueio que deseja remover?" << endl;
                getline(cin, second);
                bool found = networkManager.remove_block(first, second);
                if(found){
                    cout<< "Bloqueio removido entre essas 2 estações com sucesso.\n" << endl;
                }
                else{
                    cout<< "Não foi encontrado um troço que conecte diretamente essas 2 estações.\n" << endl;
                }
                break;
            }
            case 10: {
                string first, second;
                cout << "Qual é a primeira estação?" << endl;
                cin.ignore();
                getline(cin, first);
                cout << "Qual é a segunda estação?" << endl;
                getline(cin, second);
                int result = networkManager.max_trains_with_blocks(first, second);
                if (result == -1) {
                    cout << "Erro ao ler as estações submetidas" << endl;
                } else if (result == 0) { // quando dá 0 significa que não existe um caminho entre as duas estações
                    cout << "Nao existe um caminho, tendo em conta as limitações da ferrovia, entre essas 2 estações.\n"
                         << endl;
                } else {
                    cout << "No máximo, entre essas duas estacoes, podem passar " << result
                         << " comboios ao mesmo tempo.\n"
                         << endl;
                }
                break;
            }
            case 11: {
                int rank;
                cout << "Digite o top-k de estações mais afetadas que deseja ver" << endl;
                cin >> rank;
                networkManager.most_affected_stations(rank);
                cout << "\n\n";
                break;
            }
            case 12: {
                string first, second;
                cout << "Qual é a primeira estação?" << endl;
                cin.ignore();
                getline(cin, first);
                cout << "Qual é a segunda estação?" << endl;
                getline(cin, second);
                networkManager.bfs(first, second);
                cout << "\n\n";
                break;
            }
            case 0:
                running = false;
                break;
            default:
                cout << "[INVALID INPUT]" << endl;
                break;
        }
    }
    return 0;
}



