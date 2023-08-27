
#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "GPAI.hpp"

using namespace godot;

// Note: It is not recommended to rename that function, except you know what you are doing
void initialize_GPAI_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<GPAI>();
}

// Note: It is not recommended to rename that function, except you know what you are doing
void uninitialize_GPAI_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization.
// Note: It is not recommended to rename that function, except you know what you are doing
GDExtensionBool GDE_EXPORT GPAI_library_init(const GDExtensionInterfaceGetProcAddress p_interface, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

    init_obj.register_initializer(initialize_GPAI_module);
    init_obj.register_terminator(uninitialize_GPAI_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}

    