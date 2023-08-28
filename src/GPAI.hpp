
#ifndef GPAI_CLASS_H
#define GPAI_CLASS_H

// We don't need windows.h in this GPAI plugin but many others do, and it can
// lead to annoying situations due to the ton of macros it defines.
// So we include it and make sure CI warns us if we use something that conflicts
// with a Windows define.
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/node3d.hpp>

#include <godot_cpp/core/binder_common.hpp>

#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/navigation_region3d.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>

using namespace godot;

class GPAI : public Node3D {
    GDCLASS(GPAI, Node3D);

private:
    float vision_cone_arc;

    bool calc_los_this_frame;
    Node3D* target;
    uint32_t collision_mask = 4294967295;
    TypedArray<RID> exclude = {};
    PhysicsDirectSpaceState3D* space_state;
    bool has_los;

    NavigationRegion3D* nav_region;
    

    // ======== Debug ========
    bool debug_show_vision_cone_arc;

    void clean_up_debugs();
    void debug_toggle_vision_arc_mesh(bool show);

protected:
    static void _bind_methods();

public:
    GPAI();
    ~GPAI();

    bool use_navigation;
    NavigationAgent3D* nav_agent;

    // ======== Overrides ========
    void _ready() override;
	void _process(double delta) override;
	void _physics_process(double delta) override;
	void _enter_tree() override;
	void _exit_tree() override;

    // ======== Setters/ Getters ========
    float get_vision_cone_arc();
    void set_vision_cone_arc(float arc);
    PhysicsDirectSpaceState3D* get_space_state();
    void set_space_state(PhysicsDirectSpaceState3D* state);
    Node3D* get_target();
    void set_target(Node3D* new_target);
    bool get_los();
    void set_los(bool los);
    bool get_show_vision_cone_arc();
    void set_show_vision_cone_arc(bool show);
    
    bool get_use_navigation();
    void set_use_navigation(bool use_nav);
    NavigationRegion3D* get_nav_region();
    void set_nav_region(NavigationRegion3D* region);
    Vector3 get_next_navigation_pos();

    // ======== Vision ========
    bool is_inside_vision_cone(Vector3 own_look_dir);
    Dictionary get_lineofsight();

};
#endif // GPAI_CLASS_H
