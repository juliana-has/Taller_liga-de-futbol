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

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    std::cout << "\nPresiona doble Enter para continuar...";
    std::cin.ignore();
    std::cin.get();
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

//  FUNCIONES

void verTablaPosiciones(const ConfigLiga& config) {
    std::cout << "\n--- Tabla de posiciones ---\n";
    std::cout << "(Proximamente...)\n";
    pausar();
}

void registrarPartido(const ConfigLiga& config) {
    std::cout << "\n--- Registrar partido ---\n";
    std::cout << "(Proximamente...)\n";
    pausar();
}

void verHistorialJornadas() {
    std::cout << "\n--- Historial de jornadas ---\n";
    std::cout << "(Proximamente...)\n";
    pausar();
}

void verTodosLosPartidos() {
    std::cout << "\n--- Todos los partidos jugados ---\n";
    std::cout << "(Proximamente...)\n";
    pausar();
}

void verHistorialEnfrentamientos(const ConfigLiga& config) {
    std::cout << "\n--- Historial de enfrentamientos ---\n";
    std::cout << "(Proximamente...)\n";
    pausar();
}

void editarResultado(const ConfigLiga& config) {
    std::cout << "\n--- Editar resultado ---\n";
    std::cout << "(Proximamente...)\n";
    pausar();
}

//  MENU PRINCIPAL

int mostrarMenu(const std::string& nombreLiga) {
    int opcion = 0;
    std::cout << "\n";
    std::cout << "=========================================\n";
    std::cout << "   " << nombreLiga << "\n";
    std::cout << "=========================================\n";
    std::cout << "  1. Registrar resultado de un partido\n";
    std::cout << "  2. Ver historial de jornadas\n";
    std::cout << "  3. Ver todos los partidos jugados\n";
    std::cout << "  4. Historial de enfrentamientos\n";
    std::cout << "  5. Editar resultado de un partido\n";
    std::cout << "  6. Ver tabla de posiciones\n";
    std::cout << "  7. Salir\n";
    std::cout << "=========================================\n";
    std::cout << "  Elige una opcion: ";
    std::cin >> opcion;

    if (std::cin.fail() || opcion < 1 || opcion > 7) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "  Opcion invalida. Intenta de nuevo.\n";
        return 0;
    }
    std::cin.ignore(1000, '\n');
    return opcion;
}

//  MAIN

int main() {
    ConfigLiga config;

    if (!leerConfig("data/config.txt", config)) {
        std::cerr << "El programa no puede continuar sin una configuracion valida.\n";
        return 1;
    }

    int opcion = 0;
    do {
        limpiarPantalla();
        opcion = mostrarMenu(config.nombreLiga);

        switch (opcion) {
            case 1: registrarPartido(config);            break;
            case 2: verHistorialJornadas();              break;
            case 3: verTodosLosPartidos();               break;
            case 4: verHistorialEnfrentamientos(config); break;
            case 5: editarResultado(config);             break;
            case 6: verTablaPosiciones(config);          break;
            case 7: std::cout << "\nHasta luego!\n";    break;
            default: break;
        }
    } while (opcion != 7);

    return 0;
}
