#include <cstdint>

// capacidades son placeholders
typedef struct{
    uint32_t dni;
    char nombres[40];
    char apellidos[40];
    char direccion[30];
    char nacimiento[8];
    char nacionalidad[3];
    char lugarnacimiento[20];
    uint32_t telefono;
    char correo[40];
    char estadoCivil;   
} DatosPersona;