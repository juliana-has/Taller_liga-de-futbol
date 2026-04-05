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

std::string ajustar(const std::string& texto, int ancho) {
    if ((int)texto.size() >= ancho) return texto.substr(0, ancho);
    return texto + std::string(ancho - texto.size(), ' ');
}

std::string ajustar(int numero, int ancho) {
    return ajustar(std::to_string(numero), ancho);
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

//  LECTURA DE partidos.txt

std::vector<Partido> leerPartidos(const std::string& ruta) {
    std::vector<Partido> partidos;
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return partidos;

    std::string linea;
    while (std::getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string campo;
        Partido p;

        std::getline(ss, p.fecha,     '|');
        std::getline(ss, p.local,     '|');
        std::getline(ss, p.visitante, '|');
        std::getline(ss, campo, '|'); p.golesLocal     = std::stoi(campo);
        std::getline(ss, campo, '|'); p.golesVisitante = std::stoi(campo);

        partidos.push_back(p);
    }
    archivo.close();
    return partidos;
}

//  GUARDAR PARTIDO EN partidos.txt

bool guardarPartido(const std::string& ruta, const Partido& p) {
    std::ofstream archivo(ruta, std::ios::app);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo escribir en " << ruta << "\n";
        return false;
    }
    archivo << p.fecha << "|" << p.local << "|" << p.visitante << "|"
            << p.golesLocal << "|" << p.golesVisitante << "\n";
    archivo.close();
    return true;
}

//  GUARDAR JORNADA EN jornads.txt

bool guardarJornada(const std::string& ruta, int numJornada, const Partido& p) {
    std::ofstream archivo(ruta, std::ios::app);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo escribir en " << ruta << "\n";
        return false;
    }
    archivo << "JORNADA=" << numJornada << "\n";
    archivo << p.fecha << "|" << p.local << "|" << p.visitante << "|"
            << p.golesLocal << "|" << p.golesVisitante << "\n";
    archivo << "FIN_JORNADA\n";
    archivo.close();
    return true;
}

//  FUNCION CON PUNTEROS — actualiza estadisticas de un equipo

void actualizarEstadisticas(Equipo* e, int golesFavor, int golesContra,
                             const ConfigLiga& config) {
    e->jugados++;
    e->golesFavor  += golesFavor;
    e->golesContra += golesContra;
    e->diferencia   = e->golesFavor - e->golesContra;

    if (golesFavor > golesContra) {
        e->ganados++;
        e->puntos += config.puntosPorVictoria;
    } else if (golesFavor == golesContra) {
        e->empatados++;
        e->puntos += config.puntosPorEmpate;
    } else {
        e->perdidos++;
        e->puntos += config.puntosPorDerrota;
    }
}

//  CONSTRUIR TABLA

std::vector<Equipo> construirTabla(const std::vector<Partido>& partidos,
                                    const ConfigLiga& config) {
    std::vector<Equipo> tabla;
    for (const std::string& nombre : config.equipos) {
        Equipo e;
        e.nombre = nombre;
        tabla.push_back(e);
    }

    for (const Partido& p : partidos) {
        for (int i = 0; i < (int)tabla.size(); i++) {
            if (tabla[i].nombre == p.local)
                actualizarEstadisticas(&tabla[i], p.golesLocal, p.golesVisitante, config);
            if (tabla[i].nombre == p.visitante)
                actualizarEstadisticas(&tabla[i], p.golesVisitante, p.golesLocal, config);
        }
    }

    std::sort(tabla.begin(), tabla.end(), [](const Equipo& a, const Equipo& b) {
        if (a.puntos     != b.puntos)     return a.puntos     > b.puntos;
        if (a.diferencia != b.diferencia) return a.diferencia > b.diferencia;
        return a.golesFavor > b.golesFavor;
    });

    return tabla;
}

//  MOSTRAR TABLA EN CONSOLA

void mostrarTabla(const std::vector<Equipo>& tabla) {
    std::cout << "\n";
    std::cout << ajustar("#",  3)
              << ajustar("Equipo", 22)
              << ajustar("PJ", 5)
              << ajustar("PG", 5)
              << ajustar("PE", 5)
              << ajustar("PP", 5)
              << ajustar("GF", 5)
              << ajustar("GC", 5)
              << ajustar("DG", 6)
              << ajustar("PTS", 5)
              << "\n";
    std::cout << std::string(66, '-') << "\n";

    for (int i = 0; i < (int)tabla.size(); i++) {
        const Equipo& e = tabla[i];
        std::string dg = (e.diferencia >= 0 ? "+" : "") + std::to_string(e.diferencia);
        std::cout << ajustar(i + 1,          3)
                  << ajustar(e.nombre,       22)
                  << ajustar(e.jugados,       5)
                  << ajustar(e.ganados,       5)
                  << ajustar(e.empatados,     5)
                  << ajustar(e.perdidos,      5)
                  << ajustar(e.golesFavor,    5)
                  << ajustar(e.golesContra,   5)
                  << ajustar(dg,              6)
                  << ajustar(e.puntos,        5)
                  << "\n";
    }
    std::cout << "\n";
}

//  VER TABLA DE POSICIONES

void verTablaPosiciones(const ConfigLiga& config) {
    std::vector<Partido> partidos = leerPartidos("data/partidos.txt");
    std::vector<Equipo> tabla = construirTabla(partidos, config);
    mostrarTabla(tabla);
    pausar();
}

//  REGISTRAR PARTIDO

void registrarPartido(const ConfigLiga& config) {
    std::cout << "\n--- Registrar resultado de un partido ---\n\n";

    std::cout << "Equipos disponibles:\n";
    for (int i = 0; i < (int)config.equipos.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << config.equipos[i] << "\n";
    }

    int idxLocal = 0, idxVisitante = 0;
    std::cout << "\nSelecciona el equipo LOCAL (numero): ";
    std::cin >> idxLocal; idxLocal--;

    if (idxLocal < 0 || idxLocal >= (int)config.equipos.size()) {
        std::cout << "Numero de equipo invalido.\n";
        pausar(); return;
    }

    std::cout << "Selecciona el equipo VISITANTE (numero): ";
    std::cin >> idxVisitante; idxVisitante--;

    if (idxVisitante < 0 || idxVisitante >= (int)config.equipos.size()) {
        std::cout << "Numero de equipo invalido.\n";
        pausar(); return;
    }

    if (idxLocal == idxVisitante) {
        std::cout << "Error: el equipo local y visitante no pueden ser el mismo.\n";
        pausar(); return;
    }

    int golesLocal = 0, golesVisitante = 0;
    std::cout << "Goles del equipo local (" << config.equipos[idxLocal] << "): ";
    std::cin >> golesLocal;
    std::cout << "Goles del equipo visitante (" << config.equipos[idxVisitante] << "): ";
    std::cin >> golesVisitante;

    if (golesLocal < 0 || golesVisitante < 0) {
        std::cout << "Error: los goles no pueden ser negativos.\n";
        pausar(); return;
    }

    std::string fecha;
    std::cout << "Fecha del partido (ej: 2025-04-05): ";
    std::cin.ignore();
    std::getline(std::cin, fecha);
    fecha = trim(fecha);

    Partido p;
    p.fecha          = fecha;
    p.local          = config.equipos[idxLocal];
    p.visitante      = config.equipos[idxVisitante];
    p.golesLocal     = golesLocal;
    p.golesVisitante = golesVisitante;

    std::vector<Partido> anteriores = leerPartidos("data/partidos.txt");
    int numJornada = (int)anteriores.size() + 1;

    bool okPartido = guardarPartido("data/partidos.txt", p);
    bool okJornada = guardarJornada("data/fechas.txt", numJornada, p);

    if (okPartido && okJornada) {
        std::cout << "\nPartido registrado correctamente!\n";
        std::cout << p.local << " " << p.golesLocal
                  << " - " << p.golesVisitante << " " << p.visitante << "\n";
    } else {
        std::cout << "Hubo un error al guardar el partido.\n";
    }
    pausar();
}

//  ESPACIOS

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
