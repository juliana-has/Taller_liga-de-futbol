#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

//  STRUCTS

struct ConfigLiga {
    std::string nombreLiga;
    int puntosPorVictoria;
    int puntosPorEmpate;
    int puntosPorDerrota;
    std::vector<std::string> equipos;
};

struct Partido {
    std::string fecha;
    std::string local;
    std::string visitante;
    int golesLocal;
    int golesVisitante;
};

struct Equipo {
    std::string nombre;
    int jugados       = 0;
    int ganados       = 0;
    int empatados     = 0;
    int perdidos      = 0;
    int golesFavor    = 0;
    int golesContra   = 0;
    int diferencia    = 0;
    int puntos        = 0;
};

//  UTILIDADES

std::string trim(const std::string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    size_t fin = s.find_last_not_of(" \t\r\n");
    if (ini == std::string::npos) return "";
    return s.substr(ini, fin - ini + 1);
}


//  LECTURA DE config.txt

bool leerConfig(const std::string& ruta, ConfigLiga& config) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir " << ruta << "\n";
        return false;
    }

    bool tieneLiga = false, tieneVictoria = false;
    bool tieneEmpate = false, tieneDerrota = false;

    std::string linea;
    while (std::getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;

        size_t pos = linea.find('=');
        if (pos == std::string::npos) continue;

        std::string clave = trim(linea.substr(0, pos));
        std::string valor = trim(linea.substr(pos + 1));

        if      (clave == "liga")            { config.nombreLiga = valor;                    tieneLiga = true; }
        else if (clave == "puntos_victoria") { config.puntosPorVictoria = std::stoi(valor); tieneVictoria = true; }
        else if (clave == "puntos_empate")   { config.puntosPorEmpate = std::stoi(valor);   tieneEmpate = true; }
        else if (clave == "puntos_derrota")  { config.puntosPorDerrota = std::stoi(valor);  tieneDerrota = true; }
        else if (clave == "equipo")          { if (!valor.empty()) config.equipos.push_back(valor); }
    }
    archivo.close();

    if (!tieneLiga || !tieneVictoria || !tieneEmpate || !tieneDerrota) {
        std::cerr << "Error: config.txt esta incompleto.\n";
        return false;
    }
    if (config.equipos.empty()) {
        std::cerr << "Error: config.txt no tiene equipos.\n";
        return false;
    }
    return true;
}

//  MAIN

int main() {
    ConfigLiga config;

    if (!leerConfig("data/config.txt", config)) {
        std::cerr << "El programa no puede continuar sin una configuracion valida.\n";
        return 1;
    }

    std::cout << "Liga cargada: " << config.nombreLiga << "\n";
    std::cout << "Puntos - V:" << config.puntosPorVictoria
              << "  E:" << config.puntosPorEmpate
              << "  D:" << config.puntosPorDerrota << "\n";
    std::cout << "Equipos (" << config.equipos.size() << "):\n";
    for (const std::string& e : config.equipos) {
        std::cout << "  - " << e << "\n";
    }

    return 0;
}
