#include <moai-core/headers.h>

#define bundled_lua_SIZE 0x0000008D

unsigned char bundled_lua [] = {
	0x1B, 0x4C, 0x75, 0x61, 0x51, 0x00, 0x01, 0x04, 0x04, 0x04, 0x08, 0x00,
	0x0A, 0x00, 0x00, 0x00, 0x40, 0x6D, 0x61, 0x69, 0x6E, 0x2E, 0x6C, 0x75,
	0x61, 0x00, 0x07, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x41, 0x40,
	0x00, 0x00, 0x1C, 0x40, 0x00, 0x01, 0x1E, 0x00, 0x80, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x04, 0x06, 0x00, 0x00, 0x00, 0x70, 0x72, 0x69, 0x6E, 0x74,
	0x00, 0x04, 0x1F, 0x00, 0x00, 0x00, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69,
	0x73, 0x20, 0x61, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x6F, 0x66, 0x20,
	0x62, 0x75, 0x6E, 0x64, 0x6C, 0x65, 0x64, 0x20, 0x6C, 0x75, 0x61, 0x2E,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

//----------------------------------------------------------------//
const char* _bundled_lua_reader ( lua_State* L, void* data, size_t* size ) {
	UNUSED ( data );
	
	( *size ) = bundled_lua_SIZE;
	return ( const char* )bundled_lua;
}

//----------------------------------------------------------------//
int work_bundled_lua ( int argc, char **argv ) {
	UNUSED ( argc );
	UNUSED ( argv );
	
	//zl_init ();
	//
	//lua_State* L = lua_open ();
	//luaL_openlibs ( L );
	//
	//lua_load ( L, _bundled_lua_reader, 0, "");
	//lua_call ( L, 0, 0 );
	//
	//lua_close ( L );
	//zl_cleanup ();
	return 0;
}