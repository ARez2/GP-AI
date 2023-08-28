
#include "GPAI.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>

using namespace godot;
// ======================= Init/ Deinit =======================
GPAI::GPAI() {
    vision_cone_arc = 60.0;
    target = nullptr;
    debug_show_vision_cone_arc = false;
    nav_use_navigation = false;
    nav_agent = nullptr;
    nav_use_line_of_sight = true;
}
GPAI::~GPAI() {
}

// ======================= Overrides =======================
void GPAI::_ready() {
    calc_los_this_frame = UtilityFunctions::randi() % 2 == 0;
    space_state = get_world_3d()->get_direct_space_state();
    nav_agent = Object::cast_to<NavigationAgent3D>(find_child("NavAgent"));
    UtilityFunctions::print("_ready navagent: ", nav_agent);
}

void GPAI::_process(double delta) {
}

void GPAI::_physics_process(double delta) {
    if (!is_inside_tree()) {
        return;
    }
    calc_los_this_frame = !calc_los_this_frame;
    if (calc_los_this_frame && target != nullptr) {
        has_los = get_lineofsight().is_empty();
    } else if (!calc_los_this_frame && nav_agent != nullptr && target != nullptr) {
        Vector3 new_target_pos = target->get_global_position();
        if (nav_agent->get_target_position() != new_target_pos) {
            UtilityFunctions::print("New target pos");
            nav_agent->set_target_position(new_target_pos);
        }
    }
}

void GPAI::_enter_tree() {
    clean_up_debugs();
}
void GPAI::_exit_tree() {
    clean_up_debugs();
}


// ======================= Setters/ Getters =======================
float GPAI::get_vision_cone_arc() {
    return vision_cone_arc;
}
void GPAI::set_vision_cone_arc(float arc) {
    vision_cone_arc = arc;
    debug_toggle_vision_arc_mesh(debug_show_vision_cone_arc);
}

PhysicsDirectSpaceState3D* GPAI::get_space_state() {
    return space_state;
}
void GPAI::set_space_state(PhysicsDirectSpaceState3D* state) {
    space_state = state;
}

void GPAI::set_target(Node3D* new_target) {
    target = new_target;
}
Node3D* GPAI::get_target() {
    return target;
}

bool GPAI::get_los() {
    return has_los;
}
void GPAI::set_los(bool los) {
    has_los = los;
}

bool GPAI::get_show_vision_cone_arc() {
    return debug_show_vision_cone_arc;
}
void GPAI::set_show_vision_cone_arc(bool show) {
    debug_show_vision_cone_arc = show;
    debug_toggle_vision_arc_mesh(show);
}

bool GPAI::get_use_navigation() {
    return nav_use_navigation;
}
void GPAI::set_use_navigation(bool use_nav) {
    UtilityFunctions::print("Use nav: ", use_nav);
    nav_use_navigation = use_nav;
    if (!is_inside_tree()) {
        return;
    }
    if (use_nav) {
        NavigationAgent3D agent = NavigationAgent3D();
        if (nav_agent != nullptr) {
            nav_agent->queue_free();
        }
        add_child(&agent);
        agent.set_owner(get_tree()->get_edited_scene_root());
        agent.set_name("NavAgent");
        agent.set_debug_enabled(true);
        agent.set_debug_path_custom_color(Color(1.0, 0.0, 0.0));
        nav_agent = &agent;
    } else if (nav_agent != nullptr) {
        nav_agent->queue_free();
        nav_agent = nullptr;
    }
}

bool GPAI::get_use_nav_los() {
    return nav_use_line_of_sight;
}
void GPAI::set_use_nav_los(bool use_los) {
    nav_use_line_of_sight = use_los;
}

// ======================= Vision =======================
bool GPAI::is_inside_vision_cone(Vector3 own_look_dir) {
    Vector3 dir_to_point = target->get_global_position() - get_global_position();
    return UtilityFunctions::rad_to_deg(dir_to_point.angle_to(own_look_dir)) <= vision_cone_arc*0.5;
}

Dictionary GPAI::get_lineofsight() {
    Ref<PhysicsRayQueryParameters3D> params = PhysicsRayQueryParameters3D::create(get_global_position(), target->get_global_position(), collision_mask, exclude);
    return space_state->intersect_ray(params);
}


// ======================= Navigation =======================

Vector3 GPAI::get_next_navigation_pos() {
    if (target == nullptr) {
        return Vector3();
    }

    if (nav_use_line_of_sight && has_los) {
        return get_global_position() + get_global_position().direction_to(target->get_global_position());
    }

    if (nav_use_navigation && nav_agent != nullptr) {
        UtilityFunctions::print("nav_agent->get_next_path_position()");
        return nav_agent->get_next_path_position();
    }

    return Vector3();
}

// ======================= Debug =======================

void GPAI::clean_up_debugs() {
    TypedArray<Node> children = get_children();
    for (int i = 0; i < children.size(); i++) {
        Node* child = Object::cast_to<Node>(children[i]);
        if (child != nullptr && child->is_in_group("debug")) {
            child->queue_free();
        }
    };
}
void GPAI::debug_toggle_vision_arc_mesh(bool show) {
    if (!is_inside_tree()) {
        return;
    };

    TypedArray<Node> children = get_children();
    for (int i = 0; i < children.size(); i++) {
        Node3D* child = Object::cast_to<Node3D>(children[i]);
        if (child != nullptr && child->is_in_group("arc")) {
            child->queue_free();
        };
    };

    if (!show) {
        return;
    }

    BoxMesh mesh = BoxMesh();
    float arc_length = 4.0;
    mesh.set_size(Vector3(0.2, 0.2, arc_length));
    MeshInstance3D arc1 = MeshInstance3D();
    arc1.add_to_group("arc");
    arc1.add_to_group("debug");
    MeshInstance3D arc2 = MeshInstance3D();
    arc2.add_to_group("arc");
    arc2.add_to_group("debug");

    arc1.rotate_y(-UtilityFunctions::deg_to_rad(vision_cone_arc*0.5));
    arc2.rotate_y(UtilityFunctions::deg_to_rad(vision_cone_arc*0.5));
    arc1.translate_object_local(Vector3(0, 0, -arc_length*0.5));
    arc2.translate_object_local(Vector3(0, 0, -arc_length*0.5));
    
    arc1.set_mesh(&mesh);
    arc2.set_mesh(&mesh);
    add_child(&arc1);
    add_child(&arc2);
    // arc1.set_owner(get_tree()->get_edited_scene_root());
    // arc2.set_owner(get_tree()->get_edited_scene_root());
}



void GPAI::_bind_methods() {
    ClassDB::bind_method(D_METHOD("is_inside_vision_cone", "own_look_dir"), &GPAI::is_inside_vision_cone);
    ClassDB::bind_method(D_METHOD("get_lineofsight"), &GPAI::get_lineofsight);

    ClassDB::bind_method(D_METHOD("get_vision_cone_arc"), &GPAI::get_vision_cone_arc);
    ClassDB::bind_method(D_METHOD("set_vision_cone_arc", "arc"), &GPAI::set_vision_cone_arc);
    ClassDB::bind_method(D_METHOD("get_space_state"), &GPAI::get_space_state);
    ClassDB::bind_method(D_METHOD("set_space_state", "state"), &GPAI::set_space_state);
    ClassDB::bind_method(D_METHOD("get_target"), &GPAI::get_target);
    ClassDB::bind_method(D_METHOD("set_target", "new_target"), &GPAI::set_target);
    ClassDB::bind_method(D_METHOD("get_los"), &GPAI::get_los);
    ClassDB::bind_method(D_METHOD("set_los", "los"), &GPAI::set_los);
    ClassDB::bind_method(D_METHOD("get_show_vision_cone_arc"), &GPAI::get_show_vision_cone_arc);
    ClassDB::bind_method(D_METHOD("set_show_vision_cone_arc", "show"), &GPAI::set_show_vision_cone_arc);
    ClassDB::bind_method(D_METHOD("get_use_navigation"), &GPAI::get_use_navigation);
    ClassDB::bind_method(D_METHOD("set_use_navigation", "use_nav"), &GPAI::set_use_navigation);
    ClassDB::bind_method(D_METHOD("get_next_navigation_pos"), &GPAI::get_next_navigation_pos);
    ClassDB::bind_method(D_METHOD("get_use_nav_los"), &GPAI::get_use_nav_los);
    ClassDB::bind_method(D_METHOD("set_use_nav_los", "use_los"), &GPAI::set_use_nav_los);

    ClassDB::add_property("GPAI", PropertyInfo(Variant::FLOAT, "vision_cone_arc"), "set_vision_cone_arc", "get_vision_cone_arc");
    ClassDB::add_property("GPAI", PropertyInfo(Variant::OBJECT, "target", PROPERTY_HINT_NODE_TYPE, "Node3D"), "set_target", "get_target");
    ClassDB::add_property("GPAI", PropertyInfo(Variant::BOOL, "has_los"), "set_los", "get_los");
    
    // Navigation
    ClassDB::add_property_group("GPAI", "Navigation", "nav_");
    ClassDB::add_property("GPAI", PropertyInfo(Variant::BOOL, "nav_use_navigation"), "set_use_navigation", "get_use_navigation");
    ClassDB::add_property("GPAI", PropertyInfo(Variant::BOOL, "nav_use_line_of_sight"), "set_use_nav_los", "get_use_nav_los");
    
    //  Debug
    ClassDB::add_property_group("GPAI", "Debug", "debug_");
    ClassDB::add_property("GPAI", PropertyInfo(Variant::BOOL, "debug_show_vision_cone_arc"), "set_show_vision_cone_arc", "get_show_vision_cone_arc");
}