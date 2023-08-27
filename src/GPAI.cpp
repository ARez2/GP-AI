
#include "GPAI.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>

using namespace godot;
// ======================= Init/ Deinit =======================
GPAI::GPAI() {
    vision_cone_arc = 60.0;
    target = nullptr;
}
GPAI::~GPAI() {
}

// ======================= Overrides =======================
void GPAI::_ready() {
    calc_los_this_frame = UtilityFunctions::randi() % 2 == 0;
}

void GPAI::_process(double delta) {
    calc_los_this_frame = !calc_los_this_frame;

    if (calc_los_this_frame && target != nullptr) {
        has_los = has_lineofsight(get_global_position(), target->get_global_position(), true);
    }
}


// ======================= Setters/ Getters =======================
float GPAI::get_vision_cone_arc() {
    return vision_cone_arc;
}
void GPAI::set_vision_cone_arc(float arc) {
    vision_cone_arc = arc;
}

PhysicsDirectSpaceState3D* GPAI::get_space_state() {
    return space_state;
}
void GPAI::set_space_state(PhysicsDirectSpaceState3D* state) {
    space_state = state;
}


// ======================= Vision =======================
bool GPAI::is_inside_vision_cone(Vector3 own_pos, Vector3 own_look_dir, Vector3 point) {
    Vector3 dir_to_point = point - own_pos;
    return UtilityFunctions::rad_to_deg(dir_to_point.angle_to(own_look_dir)) <= vision_cone_arc*0.5;
}

Dictionary GPAI::get_lineofsight(Vector3 own_pos, Vector3 point) {
    Ref<PhysicsRayQueryParameters3D> params = PhysicsRayQueryParameters3D::create(own_pos, point, collision_mask, exclude);
    return space_state->intersect_ray(params);
}

bool GPAI::has_lineofsight(Vector3 own_pos, Vector3 point, bool use_cone_check) {
    if (use_cone_check) {
        return is_inside_vision_cone(own_pos, own_pos.direction_to(point), point) && !get_lineofsight(own_pos, point).is_empty();
    }
    return !get_lineofsight(own_pos, point).is_empty();
}



void GPAI::_bind_methods() {
    ClassDB::bind_method(D_METHOD("is_inside_vision_cone", "own_pos", "own_look_dir", "point"), &GPAI::is_inside_vision_cone);
    ClassDB::bind_method(D_METHOD("get_lineofsight", "own_pos", "point"), &GPAI::get_lineofsight, DEFVAL(4294967295U));
    ClassDB::bind_method(D_METHOD("has_lineofsight", "own_pos", "point", "use_cone_check"), &GPAI::has_lineofsight);

    ClassDB::add_property("GPAI", PropertyInfo(Variant::FLOAT, "vision_cone_arc"), "set_vision_cone_arc", "get_vision_cone_arc");
    ClassDB::add_property("GPAI", PropertyInfo(Variant::OBJECT, "space_state"), "set_space_state", "get_space_state");
}//