# Liga de la Lokura
**Estudiante:** Juliana Mendez  
---

## Descripcion

Aplicacion de consola en C++ que gestiona una liga de futbol inventada donde se pueden registrar 
partidos, fechas y goles. El programa es capaz de mostrar en pantalla una tabla, realiza desempate 
por diferencia de goles 

---

## Como compilar

se pega la direccion en power shell de donde este ubicado tu achivo cpp, siempre agregar cd antes de o sino no sirve ;( 
luego se agrega lo siguiente:
g++ -o liga src/main.cpp
donde dice liga es el nombre del archivo que le vas a dar a tu ejecutable :)


## Como ejecutar

entrar a power shell si estas en wiondows tomar la carpeta donde esta el ejecutable y la configuracion(el config.txt que 
hiciste para la liga) se ejecuta segun el nombre, yo le puse nombre de liga y se ejecuta con .exe osea: .\liga.exe

y pues si estas en visual studio el tambien compila y ejecuta solito (si te da pereza como a mi hacer todo eso en power shel)

## Formato de config.txt

El archivo usa pares clave=valor, uno por linea.
Las lineas que comienzan con # son comentarios y se ignoran.
Los equipos se listan con la clave equipo= repetida una vez por equipo.

Ejemplo:

```
# Configuracion de la liga
liga=Liga de la lokura

puntos_victoria=3
puntos_empate=1
puntos_derrota=0

equipo=Real Inquietud
equipo=Independiente esquizo
equipo=Atletico de la miseria
equipo=Reprobados FC
equipo=Falta Terapia
equipo=Era gol de Yepes
```

---

## Formato de partidos.txt

Cada linea representa un partido con campos separados por pipe (|):

```
fecha|equipo_local|equipo_visitante|goles_local|goles_visitante
```

Ejemplo:

```
2025-04-05|Real Inquietud|Reprobados FC|2|1
```

---

## Formato de fechas.txt

Cada jornada ocupa un bloque delimitado por JORNADA=N y FIN_JORNADA:

```
JORNADA=1
2025-04-05|Real Inquietud|Reprobados FC|2|1
FIN_JORNADA
```

---

## Decisiones 

siguiendo las indicaciones dadas y guias que tome de interntet 

**Funcion con puntero:** La funcion `actualizarEstadisticas` recibe un
puntero a Equipo (`Equipo*`) para modificar el objeto original directamente
sin necesidad de retornar una copia.

**Persistencia:** El programa usa append en partidos.txt y fechas.txt para
no perder datos entre ejecuciones. La tabla se recalcula cada vez leyendo
todos los partidos desde cero.

**Validaciones:** Se valida que los equipos existan en la liga, que sean
distintos, que los goles no sean negativos, y que no se registre el mismo
enfrentamiento dos veces en la misma fecha.

**Desempate multiple:** La tabla ordena por puntos, luego por diferencia
de goles y finalmente por goles a favor.

Fin.
P.D: se me muerieron todas las neuronas haciendo esto :,,,
