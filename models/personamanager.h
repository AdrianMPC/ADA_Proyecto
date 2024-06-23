#include "./dni-pos.h"
#include "./cuckohashing.h"
#include "./personamodelo.h"
class PersonaManager {
	private:
		CuckooHashing* m_cuckoo;
	public:
		PersonaManager(CuckooHashing* cuckoo);
		bool readPerson(uint64_t pos);
		bool writePerson(DatosPersona& data);
		bool deletePerson(uint32_t dni);
};
