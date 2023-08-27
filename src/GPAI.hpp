
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

protected:
    static void _bind_methods();

public:
    GPAI();
    ~GPAI();

    void _ready() override;
	void _process(double delta) override;

    bool has_los;


    float get_vision_cone_arc();
    void set_vision_cone_arc(float arc);
    PhysicsDirectSpaceState3D* get_space_state();
    void set_space_state(PhysicsDirectSpaceState3D* state);

    bool is_inside_vision_cone(Vector3 own_pos, Vector3 own_look_dir, Vector3 point);
    Dictionary get_lineofsight(Vector3 own_pos, Vector3 point);
    bool has_lineofsight(Vector3 own_pos, Vector3 point, bool use_cone_check);

};
#endif // GPAI_CLASS_H
