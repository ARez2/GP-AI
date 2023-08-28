extends Node3D

@export_group("Debug", "DEBUG")
@export var DEBUG_SHOW = false

func _ready() -> void:
	$GPAI.target = get_tree().get_first_node_in_group("target") as Node3D


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	$Indicator.visible = $GPAI.is_inside_vision_cone(-global_transform.basis.z) and $GPAI.get_los()
	$GPAI.get_next_navigation_pos()
