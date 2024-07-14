#pragma once
#include <cstdint>

// capacidades son placeholders
typedef struct{
    uint32_t dni; // 8 en char +1 null = 9
    char nombres[41]; // Se reserva espacio para 40 caracteres más el carácter nulo \0
    char apellidos[41]; // Se reserva espacio para 40 caracteres más el carácter nulo \0
    char direccion[31]; // Se reserva espacio para 30 caracteres más el carácter nulo \0
    char nacimiento[9]; // Se reserva espacio para 8 caracteres más el carácter nulo \0
    char nacionalidad[4]; // Se reserva espacio para 3 caracteres más el carácter nulo \0
    char lugarnacimiento[21]; // Se reserva espacio para 20 caracteres más el carácter nulo \0
    uint32_t telefono; // 9 en char +1 null = 10
    char correo[41]; // Se reserva espacio para 40 caracteres más el carácter nulo \0
    char estadoCivil[2]; // Se usa solo un char para el estado civil + carácter nulo \0
} DatosPersona;