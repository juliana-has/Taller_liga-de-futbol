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

// Rellena un string con espacios hasta el ancho indicado
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

//  GUARDAR JORNADA EN fechas.txt  

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

    // Desempate multiple
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
        std::cout << ajustar(i + 1,      3)
                  << ajustar(e.nombre,  22)
                  << ajustar(e.jugados,  5)
                  << ajustar(e.ganados,  5)
                  << ajustar(e.empatados,5)
                  << ajustar(e.perdidos, 5)
                  << ajustar(e.golesFavor,  5)
                  << ajustar(e.golesContra, 5)
                  << ajustar(dg,         6)
                  << ajustar(e.puntos,   5)
                  << "\n";
    }
    std::cout << "\n";
}

//  DETECCION DE PARTIDO DUPLICADO  

bool esPartidoDuplicado(const std::vector<Partido>& partidos,
                         const std::string& local, const std::string& visitante,
                         const std::string& fecha) {
    for (const Partido& p : partidos) {
        if (p.fecha == fecha &&
            ((p.local == local && p.visitante == visitante) ||
             (p.local == visitante && p.visitante == local))) {
            return true;
        }
    }
    return false;
}

//  HISTORIAL DE ENFRENTAMIENTOS  
void verHistorialEnfrentamientos(const ConfigLiga& config) {
    std::cout << "\n--- Historial de enfrentamientos ---\n\n";
    std::cout << "Equipos disponibles:\n";
    for (int i = 0; i < (int)config.equipos.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << config.equipos[i] << "\n";
    }

    int idx1 = 0, idx2 = 0;
    std::cout << "\nSelecciona el primer equipo (numero): ";
    std::cin >> idx1; idx1--;
    std::cout << "Selecciona el segundo equipo (numero): ";
    std::cin >> idx2; idx2--;
    std::cin.ignore(1000, '\n');

    if (idx1 < 0 || idx1 >= (int)config.equipos.size() ||
        idx2 < 0 || idx2 >= (int)config.equipos.size()) {
        std::cout << "Numero de equipo invalido.\n";
        pausar(); return;
    }
    if (idx1 == idx2) {
        std::cout << "Debes elegir dos equipos distintos.\n";
        pausar(); return;
    }

    std::string eq1 = config.equipos[idx1];
    std::string eq2 = config.equipos[idx2];
    std::vector<Partido> partidos = leerPartidos("data/partidos.txt");

    std::cout << "\nEnfrentamientos entre " << eq1 << " y " << eq2 << ":\n";
    std::cout << std::string(50, '-') << "\n";

    int encontrados = 0, ganados1 = 0, ganados2 = 0, empates = 0;

    for (const Partido& p : partidos) {
        bool eq1esLocal = (p.local == eq1 && p.visitante == eq2);
        bool eq2esLocal = (p.local == eq2 && p.visitante == eq1);

        if (eq1esLocal || eq2esLocal) {
            encontrados++;
            std::cout << p.fecha << "  "
                      << p.local << " " << p.golesLocal
                      << " - " << p.golesVisitante << " "
                      << p.visitante << "\n";

            if (p.golesLocal > p.golesVisitante) {
                if (eq1esLocal) ganados1++; else ganados2++;
            } else if (p.golesLocal < p.golesVisitante) {
                if (eq1esLocal) ganados2++; else ganados1++;
            } else {
                empates++;
            }
        }
    }

    if (encontrados == 0) {
        std::cout << "No hay enfrentamientos registrados entre estos equipos.\n";
    } else {
        std::cout << std::string(50, '-') << "\n";
        std::cout << "Total: " << encontrados << " partido(s)\n";
        std::cout << eq1 << ": " << ganados1 << " victoria(s)\n";
        std::cout << eq2 << ": " << ganados2 << " victoria(s)\n";
        std::cout << "Empates: " << empates << "\n";
    }
    pausar();
}

//  EDICION DE RESULTADO  

void editarResultado(const ConfigLiga& config) {
    std::cout << "\n--- Editar resultado de un partido ---\n\n";
    std::vector<Partido> partidos = leerPartidos("data/partidos.txt");

    if (partidos.empty()) {
        std::cout << "No hay partidos registrados para editar.\n";
        pausar(); return;
    }

    for (int i = 0; i < (int)partidos.size(); i++) {
        const Partido& p = partidos[i];
        std::cout << "  " << (i + 1) << ". "
                  << p.fecha << "  "
                  << p.local << " " << p.golesLocal
                  << " - " << p.golesVisitante << " "
                  << p.visitante << "\n";
    }

    int idx = 0;
    std::cout << "\nSelecciona el numero del partido a editar: ";
    std::cin >> idx; idx--;

    if (idx < 0 || idx >= (int)partidos.size()) {
        std::cout << "Numero invalido.\n";
        pausar(); return;
    }

    std::cout << "Nuevo marcador para: "
              << partidos[idx].local << " vs " << partidos[idx].visitante << "\n";
    std::cout << "Goles " << partidos[idx].local << ": ";
    std::cin >> partidos[idx].golesLocal;
    std::cout << "Goles " << partidos[idx].visitante << ": ";
    std::cin >> partidos[idx].golesVisitante;
    std::cin.ignore(1000, '\n');

    if (partidos[idx].golesLocal < 0 || partidos[idx].golesVisitante < 0) {
        std::cout << "Error: los goles no pueden ser negativos.\n";
        pausar(); return;
    }

    std::ofstream archivo("data/partidos.txt");
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo reescribir partidos.txt\n";
        pausar(); return;
    }
    for (const Partido& p : partidos) {
        archivo << p.fecha << "|" << p.local << "|" << p.visitante << "|"
                << p.golesLocal << "|" << p.golesVisitante << "\n";
    }
    archivo.close();

    std::cout << "\nResultado actualizado correctamente!\n";
    std::cout << partidos[idx].local << " " << partidos[idx].golesLocal
              << " - " << partidos[idx].golesVisitante << " "
              << partidos[idx].visitante << "\n";
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

    std::vector<Partido> anteriores = leerPartidos("data/partidos.txt");

    // detectar partido duplicado
    if (esPartidoDuplicado(anteriores, config.equipos[idxLocal],
                           config.equipos[idxVisitante], fecha)) {
        std::cout << "\nAdvertencia: este enfrentamiento ya fue registrado en la misma fecha.\n";
        std::cout << "No se puede registrar un partido duplicado.\n";
        pausar(); return;
    }

    Partido p;
    p.fecha          = fecha;
    p.local          = config.equipos[idxLocal];
    p.visitante      = config.equipos[idxVisitante];
    p.golesLocal     = golesLocal;
    p.golesVisitante = golesVisitante;

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

//  VER TABLA DE POSICIONES  

void verTablaPosiciones(const ConfigLiga& config) {
    std::vector<Partido> partidos = leerPartidos("data/partidos.txt");
    std::vector<Equipo> tabla = construirTabla(partidos, config);
    mostrarTabla(tabla);
    pausar();
}

//  VER HISTORIAL DE JORNADAS
void verHistorialJornadas() {
    std::cout << "\n--- Historial de jornadas ---\n\n";
    std::ifstream archivo("data/fechas.txt");

    if (!archivo.is_open()) {
        std::cout << "No hay jornadas registradas todavia.\n";
        pausar(); return;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty()) continue;

        if (linea.substr(0, 8) == "JORNADA=") {
            std::cout << "\n[ Jornada " << linea.substr(8) << " ]\n";
        } else if (linea == "FIN_JORNADA") {
            std::cout << std::string(30, '-') << "\n";
        } else {
            std::stringstream ss(linea);
            std::string fecha, local, visitante, gl, gv;
            std::getline(ss, fecha,     '|');
            std::getline(ss, local,     '|');
            std::getline(ss, visitante, '|');
            std::getline(ss, gl,        '|');
            std::getline(ss, gv,        '|');
            std::cout << "  " << fecha << "  "
                      << local << " " << gl
                      << " - " << gv << " " << visitante << "\n";
        }
    }
    archivo.close();
    pausar();
}

//  VER TODOS LOS PARTIDOS
void verTodosLosPartidos() {
    std::cout << "\n--- Todos los partidos jugados ---\n\n";
    std::vector<Partido> partidos = leerPartidos("data/partidos.txt");

    if (partidos.empty()) {
        std::cout << "No hay partidos registrados todavia.\n";
        pausar(); return;
    }

    for (int i = 0; i < (int)partidos.size(); i++) {
        const Partido& p = partidos[i];
        std::cout << (i + 1) << ". "
                  << p.fecha << "  "
                  << p.local << " " << p.golesLocal
                  << " - " << p.golesVisitante << " "
                  << p.visitante << "\n";
    }
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

int main() {
    ConfigLiga config;

    if (!leerConfig("data/config.txt", config)) {
        std::cerr << "ALGO HICISTE MAL, EL PROGRAMA NO PUEDE SEGUIR.\n";
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
