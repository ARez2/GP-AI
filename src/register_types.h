
#ifndef GPAI_REGISTER_TYPES_H
#define GPAI_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// Note: It is not recommended to rename that function, except you know what you are doing
void initialize_GPAI_module(ModuleInitializationLevel p_level);
// Note: It is not recommended to rename that function, except you know what you are doing
void uninitialize_GPAI_module(ModuleInitializationLevel p_level);

#endif // GPAI_REGISTER_TYPES_H